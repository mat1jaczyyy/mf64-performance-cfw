#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <stdint.h>
#include <stdbool.h>

void performance_init(void);
void performance_timer_event(void);
void performance_button_event(uint8_t index, bool down);
void performance_midi_event(uint8_t port, uint8_t t, uint8_t ch, uint8_t p, uint8_t v);

#endif