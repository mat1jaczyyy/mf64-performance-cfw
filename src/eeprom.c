// EEPROM functions for DJTechTools Midifighter
//
//   Copyright (C) 2011 DJ Techtools
//
// rjgreen 2009-05-22

 /* DJTT - MIDI Fighter 64 - Embedded Software License
 * Copyright (c) 2016: DJ Tech Tools
 * Permission is hereby granted, free of charge, to any person owning or possessing 
 * a DJ Tech-Tools MIDI Fighter 64 Hardware Device to view and modify this source 
 * code for personal use. Person may not publish, distribute, sublicense, or sell 
 * the source code (modified or un-modified). Person may not use this source code 
 * or any diminutive works for commercial purposes. The permission to use this source 
 * code is also subject to the following conditions:
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  FITNESS FOR A 
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "key.h"
#include "midi.h"
#include "display.h"
#include "eeprom.h"
#include "constants.h"

uint8_t G_EE_MIDI_OUTPUT_MODE;
uint8_t G_EE_SLEEP_TIME;

// EEPROM functions ------------------------------------------------------------

// Write an 8-bit value to EEPROM memory.
//
void eeprom_write(uint16_t address, uint8_t data)
{
    // Wait for completion of previous write
    while(EECR & (1<<EEPE)) {}  // !review: add timeout?
    // Set up Address and Data Registers
    EEAR = address & 0x0fff; // mask out 512 bytes
    EEDR = data;
    // This next critical section cannot have an interrupt occur between the
    // two writes or the write will fail and we will lose the information,
    // so disable all interrupts for a moment.
    cli();
    // Write logical one to EEMPE (Master Program Enable) to allow us to
    // write.
    EECR |= (1<<EEMPE);
    // Then within 4 cycles, initiate the eeprom write by writing to the
    // EEPE (Program Enable) strobe.
    EECR |= (1<<EEPE);
    // Reenable interrupts.
    sei();
}

// Read an 8-bit value from EEPROM memory.
//
uint8_t eeprom_read(uint16_t address)
{
    // Wait for completion of previous write
    while(EECR & (1<<EEPE)) {} // !review: add timeout?
    // Set up address register
    EEAR = address;
    // Start eeprom read by writing EERE (Read Enable)
    EECR |= (1<<EERE);
    // Return data from Data Register
    return EEDR;
}


// EEPROM settings ------------------------------------------------------------

// Set up the EEPROM system for use and read out the settings into the
// global values.
//
// This includes checking the layout version and, if the version tag written
// to the EEPROM doesn't match this software version we're running, we reset
// the EEPROM values to their default settings.
//
void eeprom_setup(void)
{
    // If our EEPROM layout has changed, reset everything.
    if (eeprom_read(EE_EEPROM_VERSION) != EEPROM_LAYOUT) {
        eeprom_factory_reset();
    }
	else {
	}

    // Read the EEPROM into the global settings.
    G_EE_MIDI_CHANNEL = eeprom_read(EE_MIDI_CHANNEL);
    G_EE_MIDI_VELOCITY = eeprom_read(EE_MIDI_VELOCITY);
	G_EE_MIDI_OUTPUT_MODE = eeprom_read(EE_MIDI_OUTPUT_MODE);
    G_EE_SLEEP_TIME = eeprom_read(EE_SLEEP_TIME);
}

// Return the EEPROM values to their factory default values, erasing any
// customizations you may have made. Sorry dude!
//
void eeprom_factory_reset(void)
{
     // NOTE: hardware check on first boot only occurs if the eeprom was zeroed.
    // and not every time we reflash an eeprom. That keeps it a rare event.

    eeprom_write(EE_EEPROM_VERSION, EEPROM_LAYOUT);
	// We dont want to reset the first boot check otherwise user must
	// perform button test after a factory reset
    //eeprom_write(EE_FIRST_BOOT_CHECK,      0xff); // Enable for factory firmware only

    // Reset the global variables to their default versions, as they were
    // read with their old values before the factory reset happened and they
    // could get written back if the user leaves menu mode through the exit
    // button.
    eeprom_write(EE_MIDI_CHANNEL, G_EE_MIDI_CHANNEL = 2); // MIDI channel (3)
    eeprom_write(EE_MIDI_VELOCITY, G_EE_MIDI_VELOCITY = 127); // MIDI velocity (127)
	eeprom_write(EE_COMBOS_ENABLE, 0x01); // Currently Enabled by default
	eeprom_write(EE_MIDI_OUTPUT_MODE, G_EE_MIDI_OUTPUT_MODE = MIDI_OUTPUT_MODE_NOTES_ONLY);
	eeprom_write(EE_FOUR_BANKS_MODE, 0x00);
	eeprom_write(EE_TILT_MODE, 0x02);
	eeprom_write(EE_TILT_MASK, 0xF1);
	eeprom_write(EE_ANIMATIONS, 0x04); // MF64->Geometric Animations (Default: Disabled)
 	eeprom_write(EE_TILT_SENSITIVITY, 0x1E);
 	eeprom_write(EE_PITCH_SENSITIVITY, 0x7F);
 	eeprom_write(EE_TILT_RANGE, 0x46);
 	eeprom_write(EE_PITCH_RANGE, 0x3C);
 	eeprom_write(EE_TILT_DEADZONE, 0x0C);
 	eeprom_write(EE_PITCH_DEADZONE, 0x7F);
 	eeprom_write(EE_TILT_AXIS, 0x00);
	eeprom_write(EE_PICK_SENSITIVITY, 0x40);
	eeprom_write(EE_SIDE_BANK, 0x00);
	eeprom_write(EE_SLEEP_TIME, G_EE_SLEEP_TIME = 0x3C);
	
    for (uint16_t i=0; i<NUM_BUTTONS*2; i++) {
        for (uint8_t j=0; j<3; j++) {
            eeprom_write(
                EE_COLORS_IDLE+(i*3+j),
                default_color[i < NUM_BUTTONS? COLORID_OFF : COLORID_WHITE][j]
            );
            eeprom_write(
                EE_COLORS_ACTIVE+(i*3+j),
                default_color[i < NUM_BUTTONS? COLORID_BLUE : COLORID_GREEN][j]
            );
        }
    }
}

// -----------------------------------------------------------------------------
