#include "modes/system/boot.h"
#include "fastrgb.h"
#include "modes/mode.h"

#define BOOT_NOTE_TICK     54
#define BOOT_FADE_TICK     3
#define BOOT_NOTE_LENGTH   17
#define BOOT_COLORS_LENGTH 252
#define BOOT_USER_STOP     188

// Segment boundaries in boot_notes[]
static const uint8_t boot_points[BOOT_NOTE_LENGTH + 1] = {
    0, 5, 10, 14, 18, 22, 26, 29, 32,
   34, 37, 39, 41, 43, 46, 47, 48, 49
};

// Original LP-Pro pad IDs (xy format)
static const uint8_t boot_notes[49] = {
    89, 78, 67, 56, 45,
    79, 68, 57, 46, 55,
    69, 58, 47, 36,
    59, 48, 37, 35,
    49, 38, 27, 65,
    39, 28, 26, 66,
    29, 18, 25,
    19, 17, 75,
    16, 76,
    8, 15, 77,
    7, 85,
    6, 86,
    5, 87,
    95, 88, 99,
    96,
    97,
    98
};

// State trackers
static uint8_t boot_note_elapsed = BOOT_NOTE_TICK;
static uint8_t boot_note_floor   = 0;
static uint8_t boot_note_ceil    = 0;

static uint8_t boot_fade_elapsed       = BOOT_FADE_TICK;
static uint8_t boot_fade_counter[BOOT_NOTE_LENGTH] = {0};

static bool boot_animation_started = false;

static uint8_t map_lp_to_mf(uint8_t xy) {
    int x = xy % 10;
    int y = xy / 10;

    if (x < 1) return 0xFF;
    if (x > 8) return 0xFF;
    if (y < 1) return 0xFF;
    if (y > 8) return 0xFF;

    // 0-based Index:
    x--; y--;

    return x + (y * 8);
}

// Rainbow color generator (copied & slayed from LP-Pro)
static uint8_t boot_color(uint8_t i, uint8_t c) {
    if (c == 0) {
        if (i < 63) return 0;
        if (i < 126) return i - 62;
        if (i < 189) return 63;
        if (i < 252) return 251 - i;
    }
    else if (c == 1) {
        if (i < 63) return (i + 1) / 7;
        if (i < 126) return (125 - i) / 7;
    }
    else if (c == 2) {
        if (i < 63) return i + 1;
        if (i < 126) return 63;
        if (i < 189) return 188 - i;
        if (i < 252) return 0;
    }
    return 0;
}

void boot_init() {
    fastrgb_clear();
    // Reset animation state but don't start yet
    boot_note_elapsed = BOOT_NOTE_TICK;
    boot_note_floor   = 0;
    boot_note_ceil    = 0;
    boot_fade_elapsed = BOOT_FADE_TICK;
    memset(boot_fade_counter, 0, sizeof(boot_fade_counter));
    boot_animation_started = false;
}

void boot_start_animation() {
    boot_animation_started = true;
}

void boot_timer_event() {
    // Don't start animation until explicitly started
    if (!boot_animation_started) {
        return;
    }
    
    // Advance note segments
    if (++boot_note_elapsed >= BOOT_NOTE_TICK) {
        if (boot_note_ceil < BOOT_NOTE_LENGTH) 
            boot_note_ceil++;
        boot_note_elapsed = 0;
    }

    // Draw fades
    if (++boot_fade_elapsed >= BOOT_FADE_TICK) {
        if (boot_note_floor == BOOT_NOTE_LENGTH) {
            // Animation done → Performance mode
            mode_switch(MODE_PERFORMANCE);
            return;
        }

        // For each active segment
        for (uint8_t seg = boot_note_floor; seg < boot_note_ceil; seg++) {
            if (boot_fade_counter[seg] < BOOT_COLORS_LENGTH) {
                uint8_t color_step = boot_fade_counter[seg];
                // For each pad in this segment
                for (uint8_t j = 0; j < boot_points[seg+1] - boot_points[seg]; j++) {
                    uint8_t lp_id = boot_notes[boot_points[seg] + j];
                    // Skip user LED if beyond stop
                    if (lp_id == 98 && color_step > BOOT_USER_STOP) 
                        continue;
                    // Map LP-Pro → MF64
                    uint8_t mf_id = map_lp_to_mf(lp_id);
                    if (mf_id != 0xFF) {
                        uint8_t r = boot_color(color_step, 0);
                        uint8_t g = boot_color(color_step, 1);
                        uint8_t b = boot_color(color_step, 2);
                        fastrgb_single(mf_id, r, g, b);
                    }
                    
                }
                boot_fade_counter[seg]++;
            } 
            else {
                // This segment finished fading
                boot_note_floor++;
            }
        }
        boot_fade_elapsed = 0;
    }
}

void boot_button_event(uint8_t index, bool down) { }

void boot_midi_event(uint8_t port, uint8_t t, uint8_t ch, uint8_t p, uint8_t v) {
    // ignore during boot
}