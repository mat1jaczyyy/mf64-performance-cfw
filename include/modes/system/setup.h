#ifndef SETUP_H
#define SETUP_H

#include <stdint.h>
#include <stdbool.h>

void setup_init(void);
void setup_timer_event(void);
void setup_button_event(uint8_t index, bool down);
void setup_midi_event(uint8_t port, uint8_t t, uint8_t ch, uint8_t p, uint8_t v);

#endif