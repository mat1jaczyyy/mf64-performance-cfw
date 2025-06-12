#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>

#define LED_LATCH  _BV(PB0)  // Latch the state to the LEDs (pulse high)
#define LED_CLOCK  _BV(PB1)  // SPI clock pin
#define LED_MOSI   _BV(PB2)  // SPI master out slave in
#define LED_MISO   _BV(PB3)  // SPI master in slave out
#define LED_MODE   _BV(PB7)  // LED Driver mode

#define LED_BLANK  _BV(PC6)  // Turn off all LEDs
#define LED_PWM    _BV(PC7)  // LED PWM clock output

void led_setup(void) {
	// NOTE: LED_MISO remains an input port
	DDRB = LED_CLOCK + LED_MOSI + LED_LATCH + LED_MODE;
	DDRC = LED_BLANK + LED_PWM;

	// Set up Timer1 to trigger the LED_BLANK ISR.
	// Turn off the Power Reduction Timer to free up Timer1.
	PRR0 &= ~_BV(PRTIM1);
	
	// Set Timer1 to Normal Mode (no compares).
	TCCR1A = 0;
	
	// Set the Timer1 prescaler to clock/256 (CS1x=100)
	TCCR1B |= _BV(CS12);
	TCCR1B &= ~_BV(CS11);
	TCCR1B &= ~_BV(CS10);
	
	// Set up the 16-bit value that Timer1 will count up from.
	// We are incrementing the timer at 16MHz/256, so we have 62500 events a second.
	// We need an event at 70Hz which gives us 893 counts between interrupts.
	// So we set the timer to count up from 0xFFFF-(893/2) = 0xFE40.
	TCNT1 = 0xFE40;
	    
	// Don't enable Timer1 interrupt yet
	TIMSK1 &= ~(_BV(TOIE1));
	
	// TODO: Zero out all of the animation counters.
	//for (uint8_t i=0; i<4; ++i) {
	//	g_led_counter[i] = 0;
	//}
}

void led_enable(void) {
	// Enable the Timer1 Overflow Interrupt that will trigger the ISR to refresh the display
	TIMSK1 |= _BV(TOIE1);
}

ISR(TIMER1_OVF_vect) {
    // No need to guard the 16-bit write here as interrupts are turned off inside an ISR
    TCNT1 = 0;
	// TODO: led_setup uses 0xFE40? wtf? FFE0 comes out to roughly 1000Hz
    TCNT1 = 0xFFE0;
	
    // TODO: Decrement each of the animation counters.
    // for (uint8_t i=0; i<4; ++i) {
    //     if (g_led_counter[i] > 0) { --g_led_counter[i]; }
    //}
	
	// TODO: FFE0 came out to 1000Hz, this half_ms_counter would make sense if we had 2000Hz?
	//half_ms_counter +=1;
}
