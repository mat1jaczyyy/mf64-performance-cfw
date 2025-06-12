#include <stdint.h>
#include <string.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>

#define KEY_BIT    _BV(PC7)  // Key read input low bit
#define KEY_CLOCK  _BV(PD7)  // Key read clock pin
#define KEY_LATCH  _BV(PD6)  // Key latch state to shift-out registers

#define DEBOUNCE_BUFFER_SIZE 10

uint64_t debounce_buffer[DEBOUNCE_BUFFER_SIZE];  // The debounce buffer
uint64_t key_state = 0;      // Current state of the keys after debounce.
uint64_t key_up = 0;         // Key was released since last poll.
uint64_t key_down = 0;       // Key was pressed since last poll.

void key_setup(void) {
	DDRD |= KEY_CLOCK;
	DDRD |= KEY_LATCH;
	DDRC &= ~KEY_BIT;
	PORTC |= KEY_BIT; // enable pull-up resistor on key input

	// Parallal Load (PL) on the chips is triggered low, so start it off
	// high and make sure we return it there after use. The clock pulse (CP)
	// should be high before latching (according to the datasheet), so we
	// also start it off high.
	PORTD |= KEY_CLOCK;
	PORTD |= KEY_LATCH;

	// Start the debounce buffer in an empty state.
	memset(debounce_buffer, 0, sizeof(debounce_buffer));

	// Set the Timer0 prescaler to clock/256 (CS0x=100)
	TCCR0B |= _BV(CS02);
	TCCR0B &= ~_BV(CS01);
	TCCR0B &= ~_BV(CS00);
	
	// Setup TIMER0 to trigger an overflow interrupt 1000 times a second.
	// Our counter is incremented every 256 / 16000000 = 0.000016 seconds.
	// Our key debounce buffer is 10 samples, meaning we need the counter
	// to count to x where (256*10*x)/16000000 = 0.001 seconds. This gives us
	// A counter value of 62.5 (rounded to 62), but the counter increments
	// from a base number to the overflow point at 255 so we need to set
	// the counter to (255 - 62) = 193 = 0xC1
	// Strictly, we're polling the buttons at 1008Hz, but who's counting...
	// - in reality, the time spent in this interrupt also comes into play
	// -- for mf64 this number was custom adjusted to 0xD0
	TCNT0 = 0xD0;
	
	// Set the Timer0 Overflow Interrupt Enable bit.
	TIMSK0 |= _BV(TOIE0);
}

ISR(TIMER0_OVF_vect) {
	// Where to write the next value in the ring buffer.
	static uint8_t pos = 0;
	
	TCNT0 = 0xD0;
	
	// Read in all Button States
	// Latch the key, reads on a falling edge.
	PORTD |= KEY_LATCH;
	PORTD &= ~KEY_LATCH;
	
	// Latching the inputs also presents the first bit to the output
	// pin. Shift the captured bits back to the CPU.
	uint64_t value = 0;
	uint64_t bit = 0x1;
	for (uint8_t i = 0; i < 64; i++) {
		PORTD &= ~KEY_CLOCK;  // clock falling edge does nothing.
		value |= (PINC & KEY_BIT) ? 0 : bit;
		bit <<= 1;
		PORTD |= KEY_CLOCK; // clock works on the rising edge, leave it high after use.
	}
	debounce_buffer[pos] = ~value; // Note: MF64 has inverted buttons (compared to 3D). Only logically matters right here! '~'
	pos = (pos + 1) % DEBOUNCE_BUFFER_SIZE;
	
	// TODO: Check how fast TIMER0 and TIMER1 actually count with default values
	//system_time_ms += 1;
	return;
}

void key_update(void) {
	uint64_t new_state = 0xffffffffffffffff;
	
	for (uint8_t i = 0; i < DEBOUNCE_BUFFER_SIZE; ++i) {
		new_state &= debounce_buffer[i];
	}
	
	key_down = (key_state ^ new_state) & new_state;
	key_up = (key_state ^ new_state) & key_state;
	key_state = new_state;
}
