// LED Displays for DJTechTools Midifighter2
//
//   Copyright (C) 2011 DJ Techtools
//
// rgreen 2011-03-14

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

#ifndef _DISPLAY_H_INCLUDED
#define _DISPLAY_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

// Constants ------------------------------------------------------------------
#define SIXTEENTH_FLASH_STATE   0x01

#define DISPLAY_SCALING_COLOR_IN_MAX_VALUE 48  // should usually be the max value displayed
#define DISPLAY_SCALING_COLOR_OUT_MAX_VALUE 127

// - Geometric Animations
// -- Grid Properties
#define GEOMETRIC_ANIMATION_ROWS 8
#define GEOMETRIC_ANIMATION_COLS 8
#define GEOMETRIC_ANIMATION_G_LED_IDX 3
#define GEOMETRIC_ANIMATION_G_LED_LIMIT	0x1C // Time delay between animation steps = LIMIT*0.5ms
// -- Animation Types
// --- Animation Lengths by Type
#define GEOMETRIC_ANIMATION_STEPS 15


enum DefaultColorIds {
	COLORID_OFF = 0,
	COLORID_RED = 1,
	COLORID_RED_DIM = 2,
	COLORID_ORANGE = 3,
	COLORID_ORANGE_DIM = 4,
	COLORID_YELLOW = 5,
	COLORID_YELLOW_DIM = 6,
	COLORID_CHARTREUSE = 7,
	COLORID_CHARTREUSE_DIM = 8,
	COLORID_GREEN = 9,
	COLORID_GREEN_DIM = 10,
	COLORID_CYAN = 11,
	COLORID_CYAN_DIM = 12,
	COLORID_BLUE = 13,
	COLORID_BLUE_DIM = 14,
	COLORID_LAVENDER = 15,
	COLORID_LAVENDER_DIM = 16,
	COLORID_PINK = 17,
	COLORID_PINK_DIM = 18,
	COLORID_WHITE = 19
};

extern const uint8_t default_color[20][3];

// Globals --------------------------------------------------------------------

// Storage for the LED state
extern uint8_t g_display_buffer[64 * 3];
extern const uint8_t default_color[20][3];
extern const uint8_t ableton_midi_feedback_colors[128][3];

// functions ------------------------------------------------------------------
// - LED Refreshing
void default_display_run(void); 

// - Animations
void start_geometric_animation(void);
uint8_t get_button_id_from_row_column(uint8_t button_row, uint8_t button_column);

// - Sysex Configuration Extensions
void adjust_inactive_bank_leds_for_power(uint8_t* rgb);
void adjust_active_bank_leds_for_power(uint8_t* rgb);

// ----------------------------------------------------------------------------

#endif // _DISPLAY_H_INCLUDED
