#include "modes/user/performance.h"
#include "fastrgb.h"
#include "utils/midi.h"
#include "utils/palette.h"

#define DRUM_RACK_START 36

void performance_init(void) {
    // Performance-Modus initialisieren
    fastrgb_clear();
}

void performance_timer_event(void) {
    
}

void performance_button_event(uint8_t index, bool down) {
    send_midi(0x90, index + DRUM_RACK_START, down ? 127 : 0); // Note On/Off event
}

void performance_midi_event(uint8_t port, uint8_t t, uint8_t ch, uint8_t p, uint8_t v) {
    if (t == 0x9) {
        fastrgb_ableton_single(p, v);
    } else if (t == 0x8) {
        fastrgb_ableton_single(p, 0);
    }
}