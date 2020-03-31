#ifndef _EXTI_H
#define _EXTI_H

#include "chip_common.h"

/*
 * EXTI0-15 are selectable from 8 GPIO lines each EXTI0-4 have an additional line.
 * EXTI16 is connected to the PVD output
 * EXTI17 is connected to RTC Alarm event
 * EXTI18 is connected to USB OTG Wakeup event
 * EXTI19 is connected to Ethernet Wakeup event
 * EXTI20 is connected to USB OTG HS Wakeup event
 * EXTI21 is connected to RTC Tamper and Timestamp events
 * EXTI22 is connected to RTC Wakeup
 */
class ExtiPeriph {
    uint32_t IMR;
    uint32_t EMR;
    uint32_t RTSR;
    uint32_t FTSR;
    uint32_t SWIER;
    uint32_t PR;

    public:
        int mask_interrupt(const uint32_t interrupt_num) volatile;
        int unmask_interrupt(const uint32_t interrupt_num) volatile;

        int mask_event(const uint32_t interrupt_num) volatile;
        int unmask_event(const uint32_t interrupt_num) volatile;

        int set_rising_trigger(const uint32_t interrupt_num) volatile;
        int clear_rising_trigger(const uint32_t interrupt_num) volatile;

        int set_falling_trigger(const uint32_t interrupt_num) volatile;
        int clear_falling_trigger(const uint32_t interrupt_num) volatile;

        int set_swi(const uint32_t interrupt_num) volatile;
        int clear_swi(const uint32_t interrupt_num) volatile;

        int get_pending(const uint32_t interrupt_num) volatile;
        int clear_pending(const uint32_t interrupt_num) volatile;
};

extern volatile ExtiPeriph *const EXTI;

#endif /* _EXTI_H */

