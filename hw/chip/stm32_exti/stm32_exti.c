#include "stm32_exti.h"

#define EXTI_BASE (PERIPH_BASE + 0x13c00)

#define MAX_INTERRUPT_NUM 22u

#define CHECK_VALID_INTERRUPT(num) \
    do { \
        if (num > MAX_INTERRUPT_NUM) { \
            return -1; \
        } \
    } while (0)

volatile ExtiPeriph *const EXTI = reinterpret_cast<volatile ExtiPeriph *>(EXTI_BASE);

int
ExtiPeriph::mask_interrupt(const uint32_t interrupt_num) volatile
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    IMR &= ~(1u << interrupt_num);
    return 0;
}

int
ExtiPeriph::unmask_interrupt(const uint32_t interrupt_num) volatile
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    IMR |= (1u << interrupt_num);
    return 0;
}

int
ExtiPeriph::mask_event(const uint32_t interrupt_num) volatile
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    EMR &= ~(1u << interrupt_num);
    return 0;
}

int
ExtiPeriph::unmask_event(const uint32_t interrupt_num) volatile
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    EMR |= (1u << interrupt_num);
    return 0;
}

int
ExtiPeriph::set_rising_trigger(const uint32_t interrupt_num) volatile
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    RTSR |= (1u << interrupt_num);
    return 0;
}

int
ExtiPeriph::clear_rising_trigger(const uint32_t interrupt_num) volatile
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    RTSR &= ~(1u << interrupt_num);
    return 0;
}

int
ExtiPeriph::set_falling_trigger(const uint32_t interrupt_num) volatile
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    FTSR |= (1u << interrupt_num);
    return 0;
}

int
ExtiPeriph::clear_falling_trigger(const uint32_t interrupt_num) volatile
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    FTSR &= ~(1u << interrupt_num);
    return 0;
}

int
ExtiPeriph::set_swi(const uint32_t interrupt_num) volatile
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    SWIER |= (1u << interrupt_num);
    return 0;
}

int
ExtiPeriph::clear_swi(const uint32_t interrupt_num) volatile
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    SWIER &= ~(1u << interrupt_num);
    return 0;
}

int
ExtiPeriph::get_pending(const uint32_t interrupt_num) volatile
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    return (PR & (1u << interrupt_num));
}

int
ExtiPeriph::clear_pending(const uint32_t interrupt_num) volatile
{
    CHECK_VALID_INTERRUPT(interrupt_num);
    PR |= (1u << interrupt_num);
    return 0;
}

