#include "modes/system/setup.h"
#include "fastrgb.h"

void setup_init(void) {
    fastrgb_clear();
}

void setup_timer_event(void) {

}

void setup_button_event(uint8_t index, bool down) {
    if (down) {
        fastrgb_single(index, 63, 63, 63); // Weiße LED bei Tastendruck
    } else {
        // Button losgelassen - LED ausschalten
        fastrgb_single(index, 0, 0, 0);
    }
}

void setup_midi_event(uint8_t port, uint8_t t, uint8_t ch, uint8_t p, uint8_t v) { }