#ifndef _fastrgb_H_INCLUDED
#define _fastrgb_H_INCLUDED

#include <stdint.h>
#include <string.h>    // for memset()

#include "constants.h"

extern uint8_t g_fastrgb_state[NUM_BUTTONS][3];

extern void fastrgb_clear(void);

extern void fastrgb_decompress(uint8_t* d, uint8_t* end);

extern void fastrgb_list(uint8_t* d, uint8_t* end);

extern void fastrgb_single(uint8_t p, uint8_t r, uint8_t g, uint8_t b);

extern void fastrgb_single_unsafe(uint8_t p, uint8_t r, uint8_t g, uint8_t b);

extern void fastrgb_ableton_single(uint8_t p, uint8_t v);

#endif
