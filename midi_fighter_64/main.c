#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/sfr_defs.h>
#include <avr/wdt.h>

#include <LUFA/Drivers/USB/USB.h>

#include "app.h"

void setup(void) {
	// Clear Watchdog reset flag
	MCUSR &= ~_BV(WDRF);
	wdt_disable();
	
	// Disable clock prescaling for full 16MHz speed
	clock_prescale_set(clock_div_1);
	
	// Turn off JTAG, must write same bit twice in four clock cycles for this to work
	MCUCR = _BV(JTD);
	MCUCR = _BV(JTD);
	
	sei();
	
	app_init();
	USB_Init();
	
	// TODO: Display Sleep Animation on boot
	//sleep_minute_counter = G_EE_SLEEP_TIME;
}

void loop(void) {
	app_task();
	USB_USBTask();
	wdt_reset();
}

int main(void) {
	setup();
	while (1) loop();
}
