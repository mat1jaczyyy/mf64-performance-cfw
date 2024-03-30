// EEPROM functions for DJTechTools Midifighter
//
//   Copyright (C) 2009-2011 Robin Green
//
// rgreen 2009-05-22

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

#ifndef _EEPROM_H_INCLUDED
#define _EEPROM_H_INCLUDED

// Device settings

extern uint8_t G_EE_MIDI_OUTPUT_MODE;
extern uint8_t G_EE_SLEEP_TIME;


// EEPROM functions -----------------------------------------------

void eeprom_write(uint16_t address, uint8_t data);
uint8_t eeprom_read(uint16_t address);
void eeprom_factory_reset(void);
void eeprom_setup(void);


#endif // _EEPROM_H_INCLUDED
