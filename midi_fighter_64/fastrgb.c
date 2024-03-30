#include "fastrgb.h"

uint8_t g_fastrgb_state[NUM_BUTTONS][3];

const uint8_t novation_palette[128][3] = {
	{0, 0, 0}, {16, 16, 16}, {32, 32, 32}, {63, 63, 63}, {63, 15, 15}, {63, 0, 0}, {32, 0, 0}, {16, 0, 0}, {63, 46, 26}, {63, 15, 0}, {32, 8, 0}, {16, 4, 0}, {63, 43, 11}, {63, 63, 0}, {32, 32, 0}, {16, 16, 0}, {33, 63, 12}, {20, 63, 0}, {10, 32, 0}, {5, 16, 0}, {18, 63, 18}, {0, 63, 0}, {0, 32, 0}, {0, 16, 0}, {18, 63, 23}, {0, 63, 6}, {0, 32, 3}, {0, 16, 1}, {18, 63, 22}, {0, 63, 21}, {0, 32, 11}, {0, 16, 6}, {18, 63, 45}, {0, 63, 37}, {0, 32, 18}, {0, 16, 9}, {18, 48, 63}, {0, 41, 63}, {0, 21, 32}, {0, 11, 16}, {18, 33, 63}, {0, 21, 63}, {0, 11, 32}, {0, 6, 16}, {11, 9, 63}, {0, 0, 63}, {0, 0, 32}, {0, 0, 16}, {26, 13, 62}, {11, 0, 63}, {6, 0, 32}, {3, 0, 16}, {63, 15, 63}, {63, 0, 63}, {32, 0, 32}, {16, 0, 16}, {63, 16, 27}, {63, 0, 20}, {32, 0, 10}, {16, 0, 5}, {63, 3, 0}, {37, 13, 0}, {29, 20, 0}, {8, 13, 1}, {0, 14, 0}, {0, 18, 6}, {0, 5, 27}, {0, 0, 63}, {0, 17, 19}, {4, 0, 50}, {31, 31, 31}, {7, 7, 7}, {63, 0, 0}, {46, 63, 11}, {43, 58, 1}, {24, 63, 2}, {3, 34, 0}, {0, 63, 23}, {0, 41, 63}, {0, 10, 63}, {6, 0, 63}, {22, 0, 63}, {43, 6, 30}, {10, 4, 0}, {63, 12, 0}, {33, 55, 1}, {28, 63, 5}, {0, 63, 0}, {14, 63, 9}, {21, 63, 27}, {13, 63, 50}, {22, 34, 63}, {12, 20, 48}, {26, 20, 57}, {52, 7, 63}, {63, 0, 22}, {63, 17, 0}, {45, 41, 0}, {35, 63, 0}, {32, 22, 1}, {14, 10, 0}, {0, 18, 3}, {3, 19, 8}, {5, 5, 10}, {5, 7, 22}, {25, 14, 6}, {32, 0, 0}, {54, 16, 10}, {53, 18, 4}, {63, 47, 9}, {39, 55, 11}, {25, 44, 3}, {5, 5, 11}, {54, 52, 26}, {31, 58, 34}, {38, 37, 63}, {35, 25, 63}, {15, 15, 15}, {28, 28, 28}, {55, 63, 63}, {39, 0, 0}, {13, 0, 0}, {6, 51, 0}, {1, 16, 0}, {45, 43, 0}, {15, 12, 0}, {44, 20, 0}, {18, 5, 0},
};

void fastrgb_clear(void) {
	memset(g_fastrgb_state, 0, sizeof(g_fastrgb_state));
}

inline void fastrgb_set_unsafe(uint8_t p, uint8_t r, uint8_t g, uint8_t b) {
	g_fastrgb_state[p][0] = r == 0? 0 : (r + 2);
	g_fastrgb_state[p][1] = g == 0? 0 : (g + 2);
	g_fastrgb_state[p][2] = b == 0? 0 : (b + 2);
}

inline void fastrgb_set(uint8_t p, uint8_t r, uint8_t g, uint8_t b) {
	fastrgb_set_unsafe(p & BUTTON_ID_FLAGS, r & 0x3F, g & 0x3F, b & 0x3F);
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
		novation_palette[v & 0x7F][0],
		novation_palette[v & 0x7F][1],
		novation_palette[v & 0x7F][2]
	);
}
