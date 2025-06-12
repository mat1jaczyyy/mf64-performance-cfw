#ifndef KEY_H
#define KEY_H

#include <stdint.h>

extern const uint64_t key_state;
extern const uint64_t key_up;
extern const uint64_t key_down;

void key_setup(void);
void key_update(void);

#endif
