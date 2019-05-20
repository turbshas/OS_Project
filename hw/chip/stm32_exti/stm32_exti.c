#include "stm32_exti.h"

#define EXTI_BASE (PERIPH_BASE + 0x13c00)

#define MAX_INTERRUPT_NUM 22u

#define CHECK_VALID_INTERRUPT(num) \
    do { \
        if (num > MAX_INTERRUPT_NUM) { \
            return -1; \
        } \
    } while (0)

static volatile struct exti_regs *const EXTI = (void *)EXTI_BASE;

int exti_mask_interrupt(const uint32_t interrupt_num)
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    EXTI->IMR &= ~(1u << interrupt_num);
    return 0;
}

int exti_unmask_interrupt(const uint32_t interrupt_num)
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    EXTI->IMR |= (1u << interrupt_num);
    return 0;
}

int exti_mask_event(const uint32_t interrupt_num)
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    EXTI->EMR &= ~(1u << interrupt_num);
    return 0;
}

int exti_unmask_event(const uint32_t interrupt_num)
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    EXTI->EMR |= (1u << interrupt_num);
    return 0;
}

int exti_set_rising_trigger(const uint32_t interrupt_num)
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    EXTI->RTSR |= (1u << interrupt_num);
    return 0;
}

int exti_clear_rising_trigger(const uint32_t interrupt_num)
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    EXTI->RTSR &= ~(1u << interrupt_num);
    return 0;
}

int exti_set_falling_trigger(const uint32_t interrupt_num)
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    EXTI->FTSR |= (1u << interrupt_num);
    return 0;
}

int exti_clear_falling_trigger(const uint32_t interrupt_num)
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    EXTI->FTSR &= ~(1u << interrupt_num);
    return 0;
}

int exti_set_swi(const uint32_t interrupt_num)
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    EXTI->SWIER |= (1u << interrupt_num);
    return 0;
}

int exti_clear_swi(const uint32_t interrupt_num)
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    EXTI->SWIER &= ~(1u << interrupt_num);
    return 0;
}

int exti_get_pending(const uint32_t interrupt_num)
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    return (EXTI->PR & (1u << interrupt_num));
}

int exti_clear_pending(const uint32_t interrupt_num)
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    EXTI->PR |= (1u << interrupt_num);
    return 0;
}

