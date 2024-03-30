// LED Displays for DJTechTools Midifighter2
//
//   Copyright (C) 2011 DJ Techtools
//
// Robin   Green    2011-03-14
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

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "key.h"
#include "midi.h"
#include "fastrgb.h"
#include "random.h"
#include "display.h"
#include "idle.h"
//#include "accel_gyro.h"
#include "led.h"
#include "eeprom.h"
#include "config.h" // for calling Midifighter_GetIncomingUsbMidiMessages()

// Globals --------------------------------------------------------------------

// Storage for the LED state
uint8_t g_display_buffer[64 * 3]; // This can be reduced to 64: we are treating two leds as one since they refer to a single button
// - other storage !review

// Prototypes -----------------------------------------------------------------

void left_fade(const uint8_t value, uint8_t *buffer);
void right_fade(const uint8_t value, uint8_t *buffer);
void front_fade(const uint8_t value, uint8_t *buffer);
void back_fade(const uint8_t value, uint8_t *buffer);

// Locals ---------------------------------------------------------------------

// This array holds the default colors, matched to those in the utility

const uint8_t default_color[20][3] = { // This array is used to remap sysex rgb values in to a small subset of colors.
	// Listed as RGB but actual order that this is sent to the led controller is BRG
	{0x00,0x00,0x00},   // 0: Off 
	{48,0x00,0x00},		// Red
	{24,0x00,0x00},		// Dim Red
	{40,12,0x00},   	// Orange // shows pink
	{20,6,0x00},		// Dim Orange
	{32,25,0x00},   	// Yellow
	{16,12,0x00},		// Dim Yellow
	{25,32,0x00},   // Chartreuse
	{12,16,0x00},	// 8: Dim Chartreuse
	{0x00,48,0x00},   	// Green
	{0x00,24,0x00},		// Dim Green
	{0x00,30,30},   // Cyan
	{0x00,15,15},	// Dim Cyan
	{0x00,0x00,48},   	// Blue
	{0x00,0x00,24},		// Dim Blue

	{25,7,32},   // Lavender (Dark Orchid) rgb(153,50,204)
	{13,3,17},	// 16: Dim Lavender

	{36,0x00,18},   // Pink
	{18,0x00,9},	// Dim Pink
	{24,24,24},   // 19: White
};

// local functions ------------------------------------------------------------

// !review: move animations to a new file and keep this small? (animations.c and .h)
uint8_t geometric_animation_pos = GEOMETRIC_ANIMATION_STEPS;

// - this converts geometry back to button_id
uint8_t get_button_id_from_row_column(uint8_t button_row, uint8_t button_column) {
	if (button_row >= GEOMETRIC_ANIMATION_ROWS) {
		return 0xFF; // NUM_BUTTONS
	} 
	else if (button_column >= GEOMETRIC_ANIMATION_COLS) {
		return 0xFF;
	}
	else {
		uint8_t button_id = button_row*4;
		if (button_column < 4) {  // Left half of m64
			button_id = button_id + button_column;
		}
		else { // Right half of mf64
			button_id = button_id + (button_column & 0x03) + 32;
		}
		return button_id;
	}
}

// Geometric Triangle Animation (start)
void start_geometric_animation(void) {
	//bool update_animation = false;
	if (g_led_counter[1] == 0) { // !review: move this section to geometric_animation_state?
		// reset the counter.
		g_led_counter[1] = GEOMETRIC_ANIMATION_G_LED_LIMIT;
	}
	
	geometric_animation_pos = 0;
	//g_led_counter[GEOMETRIC_ANIMATION_G_LED_IDX] = GEOMETRIC_ANIMATION_G_LED_LIMIT;
}

#define GEOMETRIC_COLOR_R 0
#define GEOMETRIC_COLOR_G 0
#define GEOMETRIC_COLOR_B 48

void draw_animation(uint8_t* buffer, uint8_t start, uint8_t pos) {
	uint8_t max = 0;
	if (pos < 3) max = pos + 1;
	else if (pos < 8) max = 4;
	else if (pos < 11) max = 11 - pos;
	else return;
	
	uint8_t offset = pos < 4? pos : (4 * pos - 9);

	for (uint8_t i = 0; i < max; i++) {
		buffer[(start + offset + i * 3) * 3 + 0] = GEOMETRIC_COLOR_B;
		buffer[(start + offset + i * 3) * 3 + 1] = GEOMETRIC_COLOR_R;
		buffer[(start + offset + i * 3) * 3 + 2] = GEOMETRIC_COLOR_G;
	}
}

void geometric_animation_state(uint8_t *buffer) {
	if (geometric_animation_pos >= GEOMETRIC_ANIMATION_STEPS) // exit if animation complete
		return;

	if (g_led_counter[1] == 0) { // !review: move this section to geometric_animation_state?
		// reset the counter.
		g_led_counter[1] = GEOMETRIC_ANIMATION_G_LED_LIMIT;

		if (++geometric_animation_pos >= GEOMETRIC_ANIMATION_STEPS) // exit if animation complete
			return;
	}

	// run the animation. Let's light up some leds!

	draw_animation(buffer, 0, geometric_animation_pos);
	draw_animation(buffer, 32, geometric_animation_pos - 4);
}

// Geometric Triangle Animation (end)

void fastrgb_state(uint8_t* buffer) {
	for (uint8_t i=0; i<NUM_BUTTONS; i++) {
		buffer[i * 3 + 0] = g_fastrgb_state[i][2];
		buffer[i * 3 + 1] = g_fastrgb_state[i][0];
		buffer[i * 3 + 2] = g_fastrgb_state[i][1];
	}
}

// Global Functions -----------------------------------------------------------

void default_display_run(void)
{
	// allow the user to write colors using MIDI input.
	fastrgb_state(g_display_buffer);
	
	// Sleep Animation
	// Increment timing counters
	if (half_ms_counter >= 2000)
	{
		half_ms_counter = 0;
		one_second_counter += 1;
		if (one_second_counter >= 60)
		{
			sleep_minute_counter += 1;
			one_second_counter = 0;
		}
	}
		
	// After a certain period of inactivity display rainbow pattern until
	// next key press.
	// If the Sleep Time is set to 0 never activate the sleep mode
	//if(G_EE_SLEEP_TIME)
	//{
	//	if (sleep_minute_counter == G_EE_SLEEP_TIME)
	//	{
	//		ball_demo_setup();
	//		sleep_minute_counter +=1;
	//	}
	//	if (sleep_minute_counter > G_EE_SLEEP_TIME)
	//	{
	//		ball_demo_run(g_display_buffer);
	//	}
	//	if (g_key_down)
	//	{
	//		one_second_counter = 0;
	//		sleep_minute_counter=0;
	//	}
	//}

	if (G_EE_SLEEP_TIME) {
		if (sleep_minute_counter == G_EE_SLEEP_TIME) {
			idle_init();
			sleep_minute_counter++;
		}
		if (sleep_minute_counter > G_EE_SLEEP_TIME) {
			idle_tick(g_display_buffer);
		}
		if (g_key_down) {
			one_second_counter = 0;
			sleep_minute_counter = 0;
		}
	}
	geometric_animation_state(g_display_buffer);
}

#define LAVENDER_GREEN_LIMIT 0x24 // Can't be lavender if it has a lot of green (MF3D Patch)
#define MF3D_UTILITY_BRIGHT_COLOR_LIMIT 0x80
#define MF3D_UTILITY_DIM_COLOR_LIMIT 0x27

// Sysex Configuration Extensions -----------------------------------------------
// - !review (adust functions below): MF3D Compatibility Patch (allow 20 colors that are scaled for power)
// -- this could be updated to a simpler equation, but 
// --- you need to be very careful about power draw. 128 leds can pull up to 2-Amps
// ---- but usb devices are limited to 0.5-Amps
void adjust_inactive_bank_leds_for_power(uint8_t* rgb)
{
	uint8_t id;
	if (!rgb[0]) // No Red
	{
		if (!rgb[1]) // Blue or Off (No Red No Green)
		{
			if (!rgb[2]) { // Case: Off -> No Change actually necessary
				id = COLORID_OFF;
			}
			else { // BLUE
				if (rgb[2] == default_color[COLORID_BLUE][2]) {return;} // Patch: do not change if already assigned as MF64 Color
				id = rgb[2] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_BLUE : COLORID_BLUE_DIM;
			}
		}
		else if (!rgb[2]) // GREEN only (no red no blue)
		{
			if (rgb[1] == default_color[COLORID_GREEN][1]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[1] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_GREEN : COLORID_GREEN_DIM;
		}
		else  // No Red, Yes Green, Yes Blue: CYAN
		{
			if (rgb[1] == default_color[COLORID_CYAN][1]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[1] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_CYAN : COLORID_CYAN_DIM;
		}
	}
	else if (!rgb[1]) // Yes Red, No Green
	{
		if (!rgb[2]) // RED ONLY
		{
			if (rgb[0] == default_color[COLORID_RED][0]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[0] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_RED : COLORID_RED_DIM;
		}
		else // Red and Blue (PINK) Note Lavender has some green in it
		{
			if (rgb[0] == default_color[COLORID_PINK][0]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[0] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_PINK : COLORID_PINK_DIM;
		}
	}
	else if (!rgb[2]) // Yes Red, Yes Green, No Blue (Yellow or 
	{
		// Yellow, Orange, or Chartreuse (rx midi (value is *2) chart:5f,7f,00, yellow: 7f,5f,00, orange: 7f,22,00)
		if (rgb[0] < rgb[1]) // CHARTRUESE: more green than red 
		{
			if (rgb[1] == default_color[COLORID_CHARTREUSE][1]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[1] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_CHARTREUSE : COLORID_CHARTREUSE_DIM;
		}
		else if (rgb[0] >> 1 >= rgb[1]) // ORANGE: more than twice as much red as green
		{
			if (rgb[0] == default_color[COLORID_ORANGE][0]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[0] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_ORANGE : COLORID_ORANGE_DIM;				
		}
		else // YELLOW 
		{
			if (rgb[0] == default_color[COLORID_YELLOW][0]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[0] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_YELLOW : COLORID_YELLOW_DIM;
		}
	}
	else // White or lavender
	{
		if (rgb[1] > LAVENDER_GREEN_LIMIT) {  // White
			id = COLORID_WHITE;//rgb[0] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_WHITE
		}
		else { // Lavender
			// - If blue is bright, assume lavendar is intended to be bright
			if (rgb[2] == default_color[COLORID_LAVENDER][2]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[0] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_LAVENDER : COLORID_LAVENDER_DIM;			
		}
	}
	rgb[0] = default_color[id][0];
	rgb[1] = default_color[id][1];
	rgb[2] = default_color[id][2];
}
void adjust_active_bank_leds_for_power(uint8_t* rgb)
{
	uint8_t id;
	if (!rgb[0]) // No Red
	{
		if (!rgb[1]) // Blue or Off (No Red No Green)
		{
			if (!rgb[2]) { // Case: Off -> No Change actually necessary
				id = COLORID_OFF;
			}
			else { // BLUE
				if (rgb[2] == default_color[COLORID_BLUE][2]) {return;} // Patch: do not change if already assigned as MF64 Color
				id = rgb[2] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_BLUE : COLORID_BLUE_DIM;
			}
		}
		else if (!rgb[2]) // GREEN only (no red no blue)
		{
			if (rgb[1] == default_color[COLORID_GREEN][1]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[1] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_GREEN : COLORID_GREEN_DIM;
		}
		else  // No Red, Yes Green, Yes Blue: CYAN
		{
			if (rgb[1] == default_color[COLORID_CYAN][1]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[1] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_CYAN : COLORID_CYAN_DIM;
		}
	}
	else if (!rgb[1]) // Yes Red, No Green
	{
		if (!rgb[2]) // RED ONLY
		{
			if (rgb[0] == default_color[COLORID_RED][0]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[0] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_RED : COLORID_RED_DIM;
		}
		else // Red and Blue (PINK) Note Lavender has some green in it
		{
			if (rgb[0] == default_color[COLORID_PINK][0]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[0] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_PINK : COLORID_PINK_DIM;
		}
	}
	else if (!rgb[2]) // Yes Red, Yes Green, No Blue (Yellow or 
	{
		// Yellow, Orange, or Chartreuse (rx midi (value is *2) chart:5f,7f,00, yellow: 7f,5f,00, orange: 7f,22,00)
		if (rgb[0] < rgb[1]) // CHARTRUESE: more green than red 
		{
			if (rgb[1] == default_color[COLORID_CHARTREUSE][1]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[1] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_CHARTREUSE : COLORID_CHARTREUSE_DIM;
		}
		else if (rgb[0] >> 1 >= rgb[1]) // ORANGE: more than twice as much red as green
		{
			if (rgb[0] == default_color[COLORID_ORANGE][0]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[0] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_ORANGE : COLORID_ORANGE_DIM;				
		}
		else // YELLOW 
		{
			if (rgb[0] == default_color[COLORID_YELLOW][0]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[0] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_YELLOW : COLORID_YELLOW_DIM;
		}
	}
	else // White or lavender
	{
		if (rgb[1] > LAVENDER_GREEN_LIMIT) {  // White
			id = COLORID_WHITE;//rgb[0] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_WHITE
		}
		else { // Lavender
			// - If blue is bright, assume lavendar is intended to be bright
			if (rgb[2] == default_color[COLORID_LAVENDER][2]) {return;} // Patch: do not change if already assigned as MF64 Color
			id = rgb[0] > MF3D_UTILITY_BRIGHT_COLOR_LIMIT ? COLORID_LAVENDER : COLORID_LAVENDER_DIM;			
		}
	}
	rgb[0] = default_color[id][0];
	rgb[1] = default_color[id][1];
	rgb[2] = default_color[id][2];
}