#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>
#include <stdbool.h>

void boot_init(void);
void boot_start_animation(void);
void boot_timer_event(void);
void boot_button_event(uint8_t index, bool down);
void boot_midi_event(uint8_t port, uint8_t t, uint8_t ch, uint8_t p, uint8_t v);

#endif