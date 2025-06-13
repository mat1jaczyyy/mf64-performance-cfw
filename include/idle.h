#ifndef _idle_H_INCLUDED
#define _idle_H_INCLUDED

#include <stdint.h>
#include <string.h>             // for memset()

extern void idle_init(void);

extern void idle_tick(uint8_t* buffer);

#endif
