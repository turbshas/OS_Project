#ifndef _PWR_H
#define _PWR_H

#include "chip_common.h"

void pwr_disable_bd_write_protection(void);
void pwr_enable_bd_write_protection(void);
void pwr_init(void);

#endif

