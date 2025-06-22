#ifndef MODE_H
#define MODE_H

#include <stdint.h>
#include <stdbool.h>

#include "modes/system/boot.h"
#define MODE_BOOT 0

#include "modes/user/performance.h"
#define MODE_PERFORMANCE 1

uint8_t mode;

void (*const mode_init[2])(void);
void (*const mode_timer_event[2])(void);
void (*const mode_button_event[2])(uint8_t index, bool down);
void (*const mode_midi_event[2])(uint8_t port, uint8_t t, uint8_t ch, uint8_t p, uint8_t v);

void mode_switch(uint8_t m);
void mode_refresh(void);

#endif // MODE_H