#include "idle.h"
#include "key.h"
#include "led.h"
#include "random.h"

// adapted from https://github.com/mat1jaczyyy/lpp-performance-cfw/blob/b764a83a896cd121227f8c6986d32947276d2891/src/modes/special/idle.c

#define IDLE_SPAWN_SHIFT 7
#define IDLE_STEP_MUL 24
#define IDLE_MAXEFFECTS 3

uint32_t idle_timer;
uint8_t idle_index;

struct idle_light {
    uint32_t start;
    uint8_t x:4;
    uint8_t y:4;
    uint8_t r:1;
    uint8_t g:1;
    uint8_t b:1;
    uint8_t e:1;
};

struct idle_light idle_effects[3];

inline uint8_t idle_interp(uint8_t c, int16_t i) {
	if (i < 0) return 128;
    if (c) {
        if (i <= 63) return 63;
        else if (i <= 126) return 126 - i;
        else return 128;
    } else {
        if (i <= 63) return 63 - i;
        else if (i <= 126) return 0;
        else return 128;
    }
}

inline uint8_t idle_validate(uint8_t x, uint8_t y) {
    return 0 <= x && x <= 7 && 0 <= y && y <= 7;
}

inline uint8_t idle_topitch(uint8_t x, uint8_t y) {
    return (x << 2) | (y & 0b011) | ((y & 0b100) << 3);
}

inline void idle_set(uint8_t* buffer, uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
    if (!idle_validate(x, y))
        return;

    uint8_t p = idle_topitch(x, y);
    buffer[p * 3 + 0] = r;
    buffer[p * 3 + 1] = g;
    buffer[p * 3 + 2] = b;
}

void idle_init(void) {
    for (uint8_t i = 0; i < IDLE_MAXEFFECTS; i++)
        idle_effects[i].e = 0;
		
	idle_index = IDLE_MAXEFFECTS - 1;
	idle_timer = system_time_ms - 1;
}

void idle_tick(uint8_t* buffer) {
    uint32_t timer = system_time_ms;

    if ((idle_timer >> IDLE_SPAWN_SHIFT) != (timer >> IDLE_SPAWN_SHIFT)) {
	    if (++idle_index >= IDLE_MAXEFFECTS)
	    idle_index = 0;

	    idle_effects[idle_index].start = timer & ~((1 << IDLE_SPAWN_SHIFT) - 1);
	    idle_effects[idle_index].e = 1;
	    idle_effects[idle_index].x = random16() & 7;
	    idle_effects[idle_index].y = random16() & 7;

	    do {
		    idle_effects[idle_index].r = random16() & 1;
		    idle_effects[idle_index].g = random16() & 1;
		    idle_effects[idle_index].b = random16() & 1;
	    } while (idle_effects[idle_index].r == 0 && idle_effects[idle_index].g == 0 && idle_effects[idle_index].b == 0);
    }

    idle_timer = timer;

    memset(buffer, 0, sizeof(*buffer) * 64 * 3);

    for (uint8_t i = 0; i < IDLE_MAXEFFECTS; i++) {
        if (idle_effects[i].e) {
            for (uint8_t j = 0; j < 8; j++) {
                int16_t t = (timer - idle_effects[i].start) - (j * IDLE_STEP_MUL);

                uint8_t r = idle_interp(idle_effects[i].r, t);
                uint8_t g = idle_interp(idle_effects[i].g, t);
                uint8_t b = idle_interp(idle_effects[i].b, t);

                if (r != 128 || g != 128 || b != 128) {
                    idle_set(buffer, idle_effects[i].x + j, idle_effects[i].y, r, g, b);
                    idle_set(buffer, idle_effects[i].x - j, idle_effects[i].y, r, g, b);
                    idle_set(buffer, idle_effects[i].x, idle_effects[i].y + j, r, g, b);
                    idle_set(buffer, idle_effects[i].x, idle_effects[i].y - j, r, g, b);
                }
            }
        }
    }
}
