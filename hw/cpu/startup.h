#ifndef _STARTUP_H
#define _STARTUP_H

#define _PERIPH_DEFN(action)    \
    /*action(CRC);              \
    action(PWR);*/              \
    action(RCC);              \
    /*action(GPIO);             \
    action(SYSCFG);           \
    action(EXTI);             \
    action(DMA);*/              \
    action(ADC);              \
    /*action(DAC);              \
    action(DCMI);             \
    action(TIM);              \
    action(WDG);              \
    action(CRYP);             \
    action(RNG);              \
    action(HASH);*/             \
    action(RTC);              \
    action(I2C);              \
    action(USART);            \
    action(SPI);              \
    action(SDIO);             \
    /*action(bxCAN);            \
    action(ETH);              \
    action(OTG);              \
    action(FSMC);             \
    action(DBG)*/


#define PERIPH_INIT(name) name##_Init()
#define PERIPH_DECL(name) void name##_Init(void)

#define DECLARE_PERIPHS() _PERIPH_DEFN(PERIPH_DECL)
#define INIT_PERIPHS() _PERIPH_DEFN(PERIPH_INIT)

/* Peripheral Initializers */
DECLARE_PERIPHS();
#ifdef __STM32F4xx__
PERIPH_DECL(DMA2D);
PERIPH_DECL(LTDC);
PERIPH_DECL(SAI);
#endif

/* Code section start and end points (defined in linker script) */
extern unsigned int _DATA_ROM_START;
extern unsigned int _DATA_RAM_START;
extern unsigned int _DATA_RAM_END;
extern unsigned int _BSS_START;
extern unsigned int _BSS_END;

#define CCM_RAM_START_LOC ((void *)0x10000000)
#define CCM_RAM_END_LOC (CCM_RAM_START_LOC + (64 * 1024))

int main(void);

__attribute__((noreturn, noinline, interrupt))
static void
Default_Handler(int num)
{
    /* Get rid of unused variable warnings */
    for (int i = 0; i < num; i++) {}
    /* Loop forever */
    for ( ;; ) {}
}

#define IRQ_HANDLER_T void __attribute__((weak, interrupt("IRQ")))

/* IRQ Handler definitions */

/* ARM Handlers */
IRQ_HANDLER_T NMI_Handler(void)                     { Default_Handler(1); }
IRQ_HANDLER_T HardFault_Handler(void)               { Default_Handler(2); }
IRQ_HANDLER_T MemManage_Handler(void)               { Default_Handler(3); }
IRQ_HANDLER_T BusFault_Handler(void)                { Default_Handler(4); }
IRQ_HANDLER_T UsageFault_Handler(void)              { Default_Handler(5); }
IRQ_HANDLER_T SVCall_Handler(void)                  { Default_Handler(6); }
IRQ_HANDLER_T DebugMon_Handler(void)                { Default_Handler(7); }
IRQ_HANDLER_T PendSV_Handler(void)                  { Default_Handler(8); }
IRQ_HANDLER_T SysTick_Handler(void)                 { Default_Handler(9); }

/* External Interrupts */
IRQ_HANDLER_T WWDG_IRQHandler(void)                 { Default_Handler(10); }
IRQ_HANDLER_T PVD_IRQHandler(void)                  { Default_Handler(11); }
IRQ_HANDLER_T TAMP_STAMP_IRQHandler(void)           { Default_Handler(12); }
IRQ_HANDLER_T RTC_WKUP_IRQHandler(void)             { Default_Handler(13); }
IRQ_HANDLER_T FLASH_IRQHandler(void)                { Default_Handler(14); }
IRQ_HANDLER_T RCC_IRQHandler(void)                  { Default_Handler(15); }

/* EXTI IRQs */
IRQ_HANDLER_T EXTI0_IRQHandler(void)                { Default_Handler(16); }
IRQ_HANDLER_T EXTI1_IRQHandler(void)                { Default_Handler(17); }
IRQ_HANDLER_T EXTI2_IRQHandler(void)                { Default_Handler(18); }
IRQ_HANDLER_T EXTI3_IRQHandler(void)                { Default_Handler(19); }
IRQ_HANDLER_T EXTI4_IRQHandler(void)                { Default_Handler(20); }

/* DMA1 IRQs */
IRQ_HANDLER_T DMA1_Stream0_IRQHandler(void)         { Default_Handler(21); }
IRQ_HANDLER_T DMA1_Stream1_IRQHandler(void)         { Default_Handler(22); }
IRQ_HANDLER_T DMA1_Stream2_IRQHandler(void)         { Default_Handler(23); }
IRQ_HANDLER_T DMA1_Stream3_IRQHandler(void)         { Default_Handler(24); }
IRQ_HANDLER_T DMA1_Stream4_IRQHandler(void)         { Default_Handler(25); }
IRQ_HANDLER_T DMA1_Stream5_IRQHandler(void)         { Default_Handler(26); }
IRQ_HANDLER_T DMA1_Stream6_IRQHandler(void)         { Default_Handler(27); }

IRQ_HANDLER_T ADC_IRQHandler(void)                  { Default_Handler(28); }

/* CAN1 IRQs */
IRQ_HANDLER_T CAN1_TX_IRQHandler(void)              { Default_Handler(29); }
IRQ_HANDLER_T CAN1_RX0_IRQHandler(void)             { Default_Handler(30); }
IRQ_HANDLER_T CAN1_RX1_IRQHandler(void)             { Default_Handler(31); }
IRQ_HANDLER_T CAN1_SCE_IRQHandler(void)             { Default_Handler(32); }

IRQ_HANDLER_T EXTI9_5_IRQHandler(void)              { Default_Handler(33); }

/* TIM IRQs */
IRQ_HANDLER_T TIM1_BRK_TIM9_IRQHandler(void)        { Default_Handler(34); }
IRQ_HANDLER_T TIM1_UP_TIM10_IRQHandler(void)        { Default_Handler(35); }
IRQ_HANDLER_T TIM1_TRG_COM_TIM11_IRQHandler(void)   { Default_Handler(36); }
IRQ_HANDLER_T TIM1_CC_IRQHandler(void)              { Default_Handler(37); }
IRQ_HANDLER_T TIM2_IRQHandler(void)                 { Default_Handler(38); }
IRQ_HANDLER_T TIM3_IRQHandler(void)                 { Default_Handler(39); }
IRQ_HANDLER_T TIM4_IRQHandler(void)                 { Default_Handler(40); }

/* I2C IRQs */
IRQ_HANDLER_T I2C1_EV_IRQHandler(void)              { Default_Handler(41); }
IRQ_HANDLER_T I2C1_ER_IRQHandler(void)              { Default_Handler(42); }
IRQ_HANDLER_T I2C2_EV_IRQHandler(void)              { Default_Handler(43); }
IRQ_HANDLER_T I2C2_ER_IRQHandler(void)              { Default_Handler(44); }

/* SPI ARQs */
IRQ_HANDLER_T SPI1_IRQHandler(void)                 { Default_Handler(45); }
IRQ_HANDLER_T SPI2_IRQHandler(void)                 { Default_Handler(46); }

/* USART IRQs */
IRQ_HANDLER_T USART1_IRQHandler(void)               { Default_Handler(47); }
IRQ_HANDLER_T USART2_IRQHandler(void)               { Default_Handler(48); }
IRQ_HANDLER_T USART3_IRQHandler(void)               { Default_Handler(49); }

IRQ_HANDLER_T EXTI15_10_IRQHandler(void)            { Default_Handler(50); }
IRQ_HANDLER_T RTC_Alarm_IRQHandler(void)            { Default_Handler(51); }
IRQ_HANDLER_T OTG_FS_WKUP_IRQHandler(void)          { Default_Handler(52); }

/* More TIM IRQs */
IRQ_HANDLER_T TIM8_BRK_TIM12_IRQHandler(void)       { Default_Handler(53); }
IRQ_HANDLER_T TIM8_UP_TIM13_IRQHandler(void)        { Default_Handler(54); }
IRQ_HANDLER_T TIM8_TRG_COM_TIM14_IRQHandler(void)   { Default_Handler(55); }
IRQ_HANDLER_T TIM8_CC_IRQHandler(void)              { Default_Handler(56); }

/* Misc extra IRQs */
IRQ_HANDLER_T DMA1_Stream7_IRQHandler(void)         { Default_Handler(57); }
IRQ_HANDLER_T FSMC_IRQHandler(void)                 { Default_Handler(58); }
IRQ_HANDLER_T SDIO_IRQHandler(void)                 { Default_Handler(59); }
IRQ_HANDLER_T TIM5_IRQHandler(void)                 { Default_Handler(60); }
IRQ_HANDLER_T SPI3_IRQHandler(void)                 { Default_Handler(61); }
IRQ_HANDLER_T UART4_IRQHandler(void)                { Default_Handler(62); }
IRQ_HANDLER_T UART5_IRQHandler(void)                { Default_Handler(63); }
IRQ_HANDLER_T TIM6_DAC_IRQHandler(void)             { /*Default_Handler(64);*/ }
IRQ_HANDLER_T TIM7_IRQHandler(void)                 { /*Default_Handler(65);*/ }

/* DMA2 IRQs */
IRQ_HANDLER_T DMA2_Stream0_IRQHandler(void)         { Default_Handler(66); }
IRQ_HANDLER_T DMA2_Stream1_IRQHandler(void)         { Default_Handler(67); }
IRQ_HANDLER_T DMA2_Stream2_IRQHandler(void)         { Default_Handler(68); }
IRQ_HANDLER_T DMA2_Stream3_IRQHandler(void)         { Default_Handler(69); }
IRQ_HANDLER_T DMA2_Stream4_IRQHandler(void)         { Default_Handler(70); }

/* Ethernet IRQs */
IRQ_HANDLER_T ETH_IRQHandler(void)                  { Default_Handler(71); }
IRQ_HANDLER_T ETH_WKUP_IRQHandler(void)             { Default_Handler(72); }

/* CAN2 IRQs */
IRQ_HANDLER_T CAN2_TX_IRQHandler(void)              { Default_Handler(73); }
IRQ_HANDLER_T CAN2_RX0_IRQHandler(void)             { Default_Handler(74); }
IRQ_HANDLER_T CAN2_RX1_IRQHandler(void)             { Default_Handler(75); }
IRQ_HANDLER_T CAN2_SCE_IRQHandler(void)             { Default_Handler(76); }

IRQ_HANDLER_T OTG_FS_IRQHandler(void)               { Default_Handler(77); }

/* More DMA2 IRQs */
IRQ_HANDLER_T DMA2_Stream5_IRQHandler(void)         { Default_Handler(78); }
IRQ_HANDLER_T DMA2_Stream6_IRQHandler(void)         { Default_Handler(79); }
IRQ_HANDLER_T DMA2_Stream7_IRQHandler(void)         { Default_Handler(80); }

IRQ_HANDLER_T USART6_IRQHandler(void)               { Default_Handler(81); }

/* I2C 3 IRQs */
IRQ_HANDLER_T I2C3_EV_IRQHandler(void)              { Default_Handler(82); }
IRQ_HANDLER_T I2C3_ER_IRQHandler(void)              { Default_Handler(83); }

/* USB OTG IRQs */
IRQ_HANDLER_T OTG_HS_EP1_OUT_IRQHandler(void)       { Default_Handler(84); }
IRQ_HANDLER_T OTG_HS_EP1_IN_IRQHandler(void)        { Default_Handler(85); }
IRQ_HANDLER_T OTG_HS_WKUP_IRQHandler(void)          { Default_Handler(86); }
IRQ_HANDLER_T OTG_HS_IRQHandler(void)               { Default_Handler(87); }
IRQ_HANDLER_T DCMI_IRQHandler(void)                 { Default_Handler(88); }
IRQ_HANDLER_T CRYP_IRQHandler(void)                 { Default_Handler(89); }
IRQ_HANDLER_T HASH_RNG_IRQHandler(void)             { Default_Handler(90); }

#ifdef __STM32F4xx__
IRQ_HANDLER_T FPU_IRQHandler(void)                  { Default_Handler(91); }
#endif

#endif /* _STARTUP_H */
