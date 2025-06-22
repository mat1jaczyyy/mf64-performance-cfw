#include "modes/mode.h"

uint8_t mode = 0;
uint8_t mode_default = 0;

void (*const mode_init[2])(void) = {
    boot_init,
    performance_init,
};

void (*const mode_timer_event[2])(void) = {
    boot_timer_event,
    performance_timer_event,
};
void (*const mode_button_event[2])(uint8_t index, bool down) = {
    boot_button_event,
    performance_button_event,
};
void (*const mode_midi_event[2])(uint8_t port, uint8_t t, uint8_t ch, uint8_t p, uint8_t v) = {
    boot_midi_event,
    performance_midi_event,
};

void mode_switch(uint8_t m) {
    mode = m;
    (*mode_init[mode])();
}

void mode_refresh(void) {
    // clear led here
    // fastrgb_clear();
}