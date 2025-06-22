// MIDI send, receive and decode functions for DJTechTools MidiFighter 3D
//
//   Copyright (C) 2011 DJ Techtools
//
// Robin   Green    2009-10-17
// Michael Mitchell 2012-01-19

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

#include <string.h>  // for memset()
#include <avr/io.h>
#include <stdlib.h>
#include "constants.h"
#include "key.h"

#include "usb_descriptors.h"
#include "led/display.h"
#include "utils/midi.h"
#include "led/led_driver.h"
#include "eeprom.h"


// Global variables ------------------------------------------------------------
bool midi_clock_enabled = false;
uint8_t ticks = 0;

// Interface object for the high level LUFA MIDI Class Drivers. This gets
// passed into every MIDI call so it can potentially keep track of many
// interfaces. The Midifighter only needs the one.
#if USE_LUFA_2015 > 0
#define MIDI_STREAM_IN_EPADDR (ENDPOINT_DIR_IN | MIDI_STREAM_IN_EPNUM)
#define MIDI_STREAM_OUT_EPADDR (ENDPOINT_DIR_OUT | MIDI_STREAM_OUT_EPNUM)
static USB_ClassInfo_MIDI_Device_t s_midi_interface = {
	.Config = {
		.StreamingInterfaceNumber = 1, // 1 = audio stream, 0 = audio control
		.DataINEndpoint = 
		{
			.Address          = MIDI_STREAM_IN_EPADDR,
			.Size             = 64, // 64
			.Banks            = 1, // !review: 0,1,2,+ none has any effect on throughput, why?
		},
		.DataOUTEndpoint = 
		{
			.Address          = MIDI_STREAM_OUT_EPADDR,
			.Size             = 64, // 64
			.Banks            = 1,	// !review: 0,1,2,+ none has any effect on throughput, why? Can we set to 3ms, and then read a bunch of messages continuously?
		},
	},
};


#else
static USB_ClassInfo_MIDI_Device_t s_midi_interface = {
    .Config = {
        .StreamingInterfaceNumber = 1,
        .DataINEndpointNumber      = MIDI_STREAM_IN_EPNUM,
        .DataINEndpointSize        = MIDI_STREAM_EPSIZE,
        .DataINEndpointDoubleBank  = false,
        .DataOUTEndpointNumber     = MIDI_STREAM_OUT_EPNUM,
        .DataOUTEndpointSize       = MIDI_STREAM_EPSIZE,
        .DataOUTEndpointDoubleBank = false,
    },
};
#endif 

USB_ClassInfo_MIDI_Device_t* g_midi_interface_info;

// NOTE(rgreen): These assignments here are debug MIDI values, you should
// never see these, the actual values will be read and set from the EEPROM
// table during startup.
//
uint8_t G_EE_MIDI_CHANNEL = 14;      // MIDI channel to listen and send on (0..15)
uint8_t G_EE_MIDI_VELOCITY = 74;     // Default velocity for NoteOn (0..127)
// - velocity of MIDI notes we track: 0-63 are bank1 (G_EE_MIDI_CHANNEL)
uint8_t g_midi_note_off_counter[NUM_BUTTONS]; //  0-63 are bank 1
// - note in minimal interrupts mode, this stores system time in 8-bit, in maximum interrupt modes, each item is a ms counter

bool g_midi_sysex_is_reading = false;
bool g_midi_sysex_is_valid = false;

// MIDI functions -------------------------------------------------------------

// Initialize the MIDI key state.
void midi_setup(void)
{
    // Set up the global LUFA MIDI class interface pointer.
    g_midi_interface_info = &s_midi_interface;

    // basenote, expnote, channel and velocity have already been set up via
    // the EEPROM settings. Clear the MIDI keystate.
    memset(g_midi_note_off_counter, 0, sizeof(g_midi_note_off_counter)); // review: why do we have two*MIDI_MAX_NOTES, but only save one. Is one unused?
}

// Append a SysEx Event to the currently selected USB Endpoint. If
// the endpoint is full it will be flushed.
//
//  length       Number of bytes in the message.
//  data         SysEx message data buffer.
//
void midi_stream_sysex(const uint8_t length, uint8_t* data)
{
    //  Assign this MIDI event to cable 0.
    MIDI_EventPacket_t midi_event;
    
    //     0x2 = 2-byte System Common
    //     0x3 = 3-byte System Common
    //     0x4 = 3-byte Sysex starts or continues
    //     0x5 = 1-byte System Common or Sysex ends
    //     0x6 = 2-byte Sysex ends
    //     0x7 = 3-byte Sysex ends

    #if USE_LUFA_2015 <= 0
    const uint8_t midi_virtual_cable = 0;
    midi_event.CableNumber = midi_virtual_cable << 4;
    #endif
	
    uint8_t num = length;// + 1;
    bool first = true;
    while (num > 3) {
		#if USE_LUFA_2015 > 0
          midi_event.Event     = 0x4;
        #else
          midi_event.Command     = 0x4;
        #endif
		
        if (first) {
            first = false;
            midi_event.Data1       = *data++;
        } else {
            midi_event.Data1       = *data++;
        }
        midi_event.Data2       = *data++;
        midi_event.Data3       = *data++;
        MIDI_Device_SendEventPacket(g_midi_interface_info, &midi_event);
        num -= 3;
    }
    if (num) {
 		#if USE_LUFA_2015 > 0
 		  midi_event.Event     = 0x5;
 		#else
 		  midi_event.Command     = 0x5;
 		#endif
        //midi_event.Command      = 0x5;
        midi_event.Data1        = *data++;
        midi_event.Data2        = 0;
        midi_event.Data3        = 0;
        if (num == 2) {
			#if USE_LUFA_2015 > 0
			midi_event.Event     = 0x6;
			#else
			midi_event.Command     = 0x6;
			#endif
            //midi_event.Command  = 0x6;
            midi_event.Data2    = *data++;
        } else if (num == 3) {
            if (first) {
				#if USE_LUFA_2015 > 0
				midi_event.Event     = 0x3;
				#else
				midi_event.Command     = 0x3;
				#endif
                //midi_event.Command     = 0x3;
            } else {
				#if USE_LUFA_2015 > 0
				midi_event.Event     = 0x7;
				#else
				midi_event.Command     = 0x7;
				#endif
                //midi_event.Command  = 0x7;
            }
            midi_event.Data2    = *data++;
            midi_event.Data3    = *data++;
        }
        MIDI_Device_SendEventPacket(g_midi_interface_info, &midi_event);
    }
}

void midi_clock(void)
{
	// If not enabled enable MIDI Clock
	if(!midi_clock_enabled){display_flash_counter=0;midi_clock_enable(true);}
	if(ticks == 3)
	{
		ticks = 1;
		display_flash_counter += 1;
	}
	else
	{
		ticks += 1;
	}
}


void midi_clock_enable(bool state)
{
	if (state) 
	{
		midi_clock_enabled =  true;
	}
	else       
	{
		midi_clock_enabled =  false;
	}
}
// ----------------------------------------------------------------------------

void send_midi(uint8_t t, uint8_t p, uint8_t v) {
	MIDI_EventPacket_t midi_event;

	midi_event.Event = t >> 4;
    midi_event.Data1       = t;
    midi_event.Data2       = p & 0x7f;   // 0..127
    midi_event.Data3       = v & 0x7f; // 0..127

    MIDI_Device_SendEventPacket(g_midi_interface_info, &midi_event);
}