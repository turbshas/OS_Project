#ifndef _EXTI_H
#define _EXTI_H

#include "chip_common.h"

struct exti_regs {
    uint32_t IMR;
    uint32_t EMR;
    uint32_t RTSR;
    uint32_t FTSR;
    uint32_t SWIER;
    uint32_t PR;
};

int exti_mask_interrupt(const uint32_t interrupt_num);
int exti_unmask_interrupt(const uint32_t interrupt_num);

int exti_mask_event(const uint32_t interrupt_num);
int exti_unmask_event(const uint32_t interrupt_num);

int exti_set_rising_trigger(const uint32_t interrupt_num);
int exti_clear_rising_trigger(const uint32_t interrupt_num);

int exti_set_falling_trigger(const uint32_t interrupt_num);
int exti_clear_falling_trigger(const uint32_t interrupt_num);

int exti_set_swi(const uint32_t interrupt_num);
int exti_clear_swi(const uint32_t interrupt_num);

int exti_get_pending(const uint32_t interrupt_num);
int exti_clear_pending(const uint32_t interrupt_num);

#endif /* _EXTI_H */

