#include "sys_timer.h"
#include "sys_ctl_block.h"

void
sys_timer_init(void)
{
    SYS_CTL->initialize();
}
