#include "fastrgb.h"
#include "utils/palette.h"

// Erweiterte Framebuffer: [Button][LED (0 oder 1)][RGB (0=R, 1=G, 2=B)]
uint8_t g_fastrgb_state[NUM_BUTTONS][2][3];

void fastrgb_clear(void) {
	memset(g_fastrgb_state, 0, sizeof(g_fastrgb_state));
}

inline void fastrgb_set_led_unsafe(uint8_t p, uint8_t led, uint8_t r, uint8_t g, uint8_t b) {
	g_fastrgb_state[p][led][0] = r == 0? 0 : (r + 2);
	g_fastrgb_state[p][led][1] = g == 0? 0 : (g + 2);
	g_fastrgb_state[p][led][2] = b == 0? 0 : (b + 2);
}

inline void fastrgb_set_led(uint8_t p, uint8_t led, uint8_t r, uint8_t g, uint8_t b) {
	fastrgb_set_led_unsafe(p & BUTTON_ID_FLAGS, led & 0x01, r & 0x3F, g & 0x3F, b & 0x3F);
}

inline void fastrgb_set_unsafe(uint8_t p, uint8_t r, uint8_t g, uint8_t b) {
	fastrgb_set_led_unsafe(p, 0, r, g, b);
	fastrgb_set_led_unsafe(p, 1, r, g, b);
}

inline void fastrgb_set(uint8_t p, uint8_t r, uint8_t g, uint8_t b) {
	fastrgb_set_unsafe(p & BUTTON_ID_FLAGS, r & 0x3F, g & 0x3F, b & 0x3F);
}

void fastrgb_set_dual(uint8_t p, uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2) {
	p &= BUTTON_ID_FLAGS;
	fastrgb_set_led_unsafe(p, 0, r1 & 0x3F, g1 & 0x3F, b1 & 0x3F);
	fastrgb_set_led_unsafe(p, 1, r2 & 0x3F, g2 & 0x3F, b2 & 0x3F);
}

inline uint8_t fastrgb_get_led_r(uint8_t p, uint8_t led) {
	return g_fastrgb_state[p & BUTTON_ID_FLAGS][led & 0x01][0];
}

inline uint8_t fastrgb_get_led_g(uint8_t p, uint8_t led) {
	return g_fastrgb_state[p & BUTTON_ID_FLAGS][led & 0x01][1];
}

inline uint8_t fastrgb_get_led_b(uint8_t p, uint8_t led) {
	return g_fastrgb_state[p & BUTTON_ID_FLAGS][led & 0x01][2];
}

/*
Decompression algorithm designed to reduce stress on Windows' MIDI stack for Apollo Studio
Originally from https://github.com/mat1jaczyyy/lpp-performance-cfw/blob/b764a83a896cd121227f8c6986d32947276d2891/src/sysex/sysex.c#L61-L119
Adapted for MIDI Fighter 64

0-63 control the LED grid in Drum Rack layout.
64-95 controls both the 6-bit pitch value, as well as its symmetrically inverse LED.
96-103 controls an entire row.
104-111 controls an entire column.
112-127 controls the 6-bit pitch value, and mirrors it to all four quadrants.
*/
void fastrgb_decompress(uint8_t* d, uint8_t* end) {
	for (uint8_t* i = d; i < end;) {
		uint8_t r = *i++;
		uint8_t g = *i++;
		uint8_t b = *i++;

		uint8_t n = ((r & 0x40) >> 4) | ((g & 0x40) >> 5) | ((b & 0x40) >> 6);
		if (n == 0) n = *i++;

		r &= 0x3F;
		g &= 0x3F;
		b &= 0x3F;

		for (uint8_t j = 0; j < n; j++) {
			uint8_t x = *i++;

			if ((x & 0b01110000) != 0b01100000) {
				fastrgb_set_unsafe(x & BUTTON_ID_FLAGS, r, g, b);

				if (x & 0b01000000) {
					uint8_t x_ = ~x & BUTTON_ID_FLAGS;
					fastrgb_set_unsafe(x_, r, g, b);

					if (x & 0b00100000) {
						fastrgb_set_unsafe((x & 0b00011100) | (x_ & 0b00000011), r, g, b);
						fastrgb_set_unsafe((x & 0b00100011) | (x_ & 0b00011100), r, g, b);
					}
				}

			} else if (x & 0b00001000) {
				uint8_t col = x & (x & 0b00000100? 0b00100011 : 0b00000011);

				for (uint8_t k = 0; k < 8; k++) {
					fastrgb_set_unsafe(col | (k << 2), r, g, b);
				}

			} else {
				uint8_t row = ((x & 0b00000111) << 2);

				for (uint8_t k = 0; k < 4; k++) {
					fastrgb_set_unsafe(row | k, r, g, b);
					fastrgb_set_unsafe(row | 0b00100000 | k, r, g, b);
				}
			}
		}
	}
}

void fastrgb_list(uint8_t* d, uint8_t* end) {
	for (uint8_t* i = d; i + 3 < end; i += 4) {
		fastrgb_set(i[0], i[1], i[2], i[3]);
	}
}

void fastrgb_single(uint8_t p, uint8_t r, uint8_t g, uint8_t b) {
	fastrgb_set(p, r, g, b);
}

void fastrgb_single_unsafe(uint8_t p, uint8_t r, uint8_t g, uint8_t b) {
	fastrgb_set_unsafe(p, r, g, b);
}

void fastrgb_ableton_single(uint8_t p, uint8_t v) {
	fastrgb_set_unsafe(
		p,
		system_palettes[selected_palette][0][v & 0x7F],
		system_palettes[selected_palette][1][v & 0x7F],
		system_palettes[selected_palette][2][v & 0x7F]
	);
}

// Neue LED-spezifische Funktionen
void fastrgb_single_led(uint8_t p, uint8_t led, uint8_t r, uint8_t g, uint8_t b) {
	fastrgb_set_led(p, led, r, g, b);
}

void fastrgb_single_led_unsafe(uint8_t p, uint8_t led, uint8_t r, uint8_t g, uint8_t b) {
	fastrgb_set_led_unsafe(p, led, r, g, b);
}

void fastrgb_ableton_single_led(uint8_t p, uint8_t led, uint8_t v) {
	fastrgb_set_led_unsafe(
		p,
		led,
		system_palettes[selected_palette][0][v & 0x7F],
		system_palettes[selected_palette][1][v & 0x7F],
		system_palettes[selected_palette][2][v & 0x7F]
	);
}
