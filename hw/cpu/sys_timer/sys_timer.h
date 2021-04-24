#ifndef _SYS_TIMER_H
#define _SYS_TIMER_H

#include <stdio.h>

#include "drivers.h"

extern uint32_t main_stack[64];

void thread_1(void);
void sys_timer_init(void);

#endif /* _SYS_TIMER_H */

