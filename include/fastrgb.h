#ifndef _fastrgb_H_INCLUDED
#define _fastrgb_H_INCLUDED

#include <stdint.h>
#include <string.h>    // for memset()

#include "constants.h"

// Erweiterte Framebuffer: [Button][LED (0 oder 1)][RGB (0=R, 1=G, 2=B)]
extern uint8_t g_fastrgb_state[NUM_BUTTONS][2][3];

extern void fastrgb_clear(void);

// Neue Funktionen für einzelne LEDs
extern void fastrgb_set_led(uint8_t p, uint8_t led, uint8_t r, uint8_t g, uint8_t b);
extern void fastrgb_set_led_unsafe(uint8_t p, uint8_t led, uint8_t r, uint8_t g, uint8_t b);
extern void fastrgb_set_dual(uint8_t p, uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2);

// Getter-Funktionen für den erweiterten Framebuffer
extern uint8_t fastrgb_get_led_r(uint8_t p, uint8_t led);
extern uint8_t fastrgb_get_led_g(uint8_t p, uint8_t led);
extern uint8_t fastrgb_get_led_b(uint8_t p, uint8_t led);

// Bestehende Funktionen (jetzt für Rückwärtskompatibilität - setzen beide LEDs)
extern void fastrgb_decompress(uint8_t* d, uint8_t* end);
extern void fastrgb_list(uint8_t* d, uint8_t* end);
extern void fastrgb_single(uint8_t p, uint8_t r, uint8_t g, uint8_t b);
extern void fastrgb_single_unsafe(uint8_t p, uint8_t r, uint8_t g, uint8_t b);
extern void fastrgb_ableton_single(uint8_t p, uint8_t v);

// Neue LED-spezifische Funktionen
extern void fastrgb_single_led(uint8_t p, uint8_t led, uint8_t r, uint8_t g, uint8_t b);
extern void fastrgb_single_led_unsafe(uint8_t p, uint8_t led, uint8_t r, uint8_t g, uint8_t b);
extern void fastrgb_ableton_single_led(uint8_t p, uint8_t led, uint8_t v);

#endif
