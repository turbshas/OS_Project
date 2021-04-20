#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "startup.h"
#include "stm32_rcc.h"
#include "sys_ctl_block.h"
#include "sys_timer.h"

#include "kernel.h"

// #define _INITIAL_SP (&main_stack[63])
#define _INITIAL_SP ((void *)0x20020000) /* Will have to find a better place for this */
#define I2C1_LOC ((void *)0x40005400)

__attribute__((interrupt("IRQ")))
static void
Reset_Handler(void)
{
    /* Copy .data section from Flash to SRAM */
    unsigned *read_ptr = &_DATA_ROM_START;
    unsigned *write_ptr = &_DATA_RAM_START;
    for ( ; write_ptr < &_DATA_RAM_END; ) {
        *write_ptr++ = *read_ptr++;
    }

    /* Init .bss section with zeros */
    for (write_ptr = &_BSS_START; write_ptr < &_BSS_END; write_ptr++) {
        *write_ptr = 0;
    }

#ifdef __STM32F4xx__
    /* Initialize CCMRAM to zeros */
    for (write_ptr = CCM_RAM_START_LOC; write_ptr < CCM_RAM_END_LOC; write_ptr++) {
        *write_ptr = 0;
    }
    /* Copy .ccmram from Flash to CCMRAM */
    read_ptr = &_CCM_ROM_START;
    write_ptr = &_CCM_RAM_START;
    for ( ; write_ptr < &_CCM_RAM_END; ) {
        *write_ptr++ = *read_ptr++;
    }

#endif

    /* Run C++ static constructors */
    uintptr_t *initializer = reinterpret_cast<uintptr_t *>(&__init_array_start);
    uintptr_t *initializersEnd = reinterpret_cast<uintptr_t *>(&__init_array_end);
    while (initializer < initializersEnd) {
        /* Make sure the address we're branching to is set to run in Thumb mode */
        const uintptr_t thumbInitializer = (*initializer) | 0x1;
        FunctionPointer fp = reinterpret_cast<FunctionPointer>(thumbInitializer);
        fp();
        initializer++;
    }

    /* Start runnin boi */
    (void)main();
}

__attribute__((section ("ISR_VECTORS"))) FunctionPointer isr_vector_table[] = {
    reinterpret_cast<FunctionPointer>(_INITIAL_SP),
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,

    /* Next 4 entries are reserved */
    reinterpret_cast<FunctionPointer>(0x4e65576f), /* Pebble magic, apparently */
    NULL,
    NULL,
    NULL,
    SVCall_Handler,
    DebugMon_Handler,
    NULL,
    PendSV_Handler,
    SysTick_Handler,

    /* External Interrupts */
    WWDG_IRQHandler,                /* Window Watchdog */
    PVD_IRQHandler,                 /* PVD through EXTI Line detection */
    TAMP_STAMP_IRQHandler,          /* Tamper and Timestamps through the EXTI line */
    RTC_WKUP_IRQHandler,            /* RTC Wakeup through the EXTI line */
    FLASH_IRQHandler,
    RCC_IRQHandler,

    /* EXTI IRQs */
    EXTI0_IRQHandler,
    EXTI1_IRQHandler,
    EXTI2_IRQHandler,
    EXTI3_IRQHandler,
    EXTI4_IRQHandler,

    /* DMA1 IRQs */
    DMA1_Stream0_IRQHandler,
    DMA1_Stream1_IRQHandler,
    DMA1_Stream2_IRQHandler,
    DMA1_Stream3_IRQHandler,
    DMA1_Stream4_IRQHandler,
    DMA1_Stream5_IRQHandler,
    DMA1_Stream6_IRQHandler,

    ADC_IRQHandler,                 /* ADC1, ADC2, and ADC3 */

    /* CAN1 IRQs */
    CAN1_TX_IRQHandler,
    CAN1_RX0_IRQHandler,
    CAN1_RX1_IRQHandler,
    CAN1_SCE_IRQHandler,

    EXTI9_5_IRQHandler,             /* EXTI Line [9:5] */

    /* TIM IRQs */
    TIM1_BRK_TIM9_IRQHandler,       /* TIM1 Break and TIM9 */
    TIM1_UP_TIM10_IRQHandler,       /* TIM1 Update and TIM10 */
    TIM1_TRG_COM_TIM11_IRQHandler,  /* TIM1 Trigger and Communication and TIM11 */
    TIM1_CC_IRQHandler,             /* TIM1 Capture Compare */
    TIM2_IRQHandler,
    TIM3_IRQHandler,
    TIM4_IRQHandler,

    /* I2C IRQs */
    I2C1_EV_IRQHandler,
    I2C1_ER_IRQHandler,
    I2C2_EV_IRQHandler,
    I2C2_ER_IRQHandler,

    /* SPI ARQs */
    SPI1_IRQHandler,
    SPI2_IRQHandler,

    /* USART IRQs */
    USART1_IRQHandler,
    USART2_IRQHandler,
    USART3_IRQHandler,

    EXTI15_10_IRQHandler,           /* EXTI Line [15:10] */
    RTC_Alarm_IRQHandler,           /* RTC Alarm A and B */
    OTG_FS_WKUP_IRQHandler,         /* USB OTG through EXTI Line */

    /* More TIM IRQs */
    TIM8_BRK_TIM12_IRQHandler,
    TIM8_UP_TIM13_IRQHandler,
    TIM8_TRG_COM_TIM14_IRQHandler,
    TIM8_CC_IRQHandler,

    /* Misc extra IRQs */
    DMA1_Stream7_IRQHandler,
    FSMC_IRQHandler,
    SDIO_IRQHandler,
    TIM5_IRQHandler,
    SPI3_IRQHandler,
    UART4_IRQHandler,
    UART5_IRQHandler,
    TIM6_DAC_IRQHandler,            /* TIM6 and DAC 1 & 2 underrun errors */
    TIM7_IRQHandler,

    /* DMA2 IRQs */
    DMA2_Stream0_IRQHandler,
    DMA2_Stream1_IRQHandler,
    DMA2_Stream2_IRQHandler,
    DMA2_Stream3_IRQHandler,
    DMA2_Stream4_IRQHandler,

    /* Ethernet IRQs */
    ETH_IRQHandler,
    ETH_WKUP_IRQHandler,            /* Ethernet Wakeup through EXTI Line */

    /* CAN2 IRQs */
    CAN2_TX_IRQHandler,
    CAN2_RX0_IRQHandler,
    CAN2_RX1_IRQHandler,
    CAN2_SCE_IRQHandler,

    OTG_FS_IRQHandler,              /* USB OTG FS */

    /* More DMA2 IRQs */
    DMA2_Stream5_IRQHandler,
    DMA2_Stream6_IRQHandler,
    DMA2_Stream7_IRQHandler,

    USART6_IRQHandler,

    /* I2C 3 IRQs */
    I2C3_EV_IRQHandler,
    I2C3_ER_IRQHandler,

    /* USB OTG IRQs */
    OTG_HS_EP1_OUT_IRQHandler,      /* USB OTG HS End Point 1 Out */
    OTG_HS_EP1_IN_IRQHandler,       /* USB OTG HS End Point 1 In */
    OTG_HS_WKUP_IRQHandler,         /* USB OTG Wakeup through EXTI */
    OTG_HS_IRQHandler,
    DCMI_IRQHandler,
    CRYP_IRQHandler,
    HASH_RNG_IRQHandler,

#ifdef __STM32F4xx__
    FPU_IRQHandler,
#endif
};

static void
System_Init(void)
{
    /*
     * Only initialize things required for
     * normal operation here e.g. clocks
     */
    RCC->init();
    sys_timer_init();
}

int
main(void)
{
    System_Init();

    //try {
        ker_main();
    //} catch (...) {
        // Nothing we can do, just proceed to spin loop
    //}

    for ( ;; ) {}

    return 0;
}
