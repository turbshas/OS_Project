#ifndef _DRIVERS_H
#define _DRIVERS_H

#include "usart_driver.h"

/* TODO: these chip drivers.
 * should move these somewhere else,
 * but I can do that later
#include "stm32_adc.h"
#include "stm32_bxcan.h"
#include "stm32_crc.h"
#include "stm32_cryp.h"
#include "stm32_dac.h"
#include "stm32_dbg.h"
#include "stm32_dcmi.h"
#include "stm32_dma.h"
#include "stm32_eth.h"
#include "stm32_exti.h"
#include "stm32_fsmc.h"
#include "stm32_gpio.h"
#include "stm32_hash.h"
#include "stm32_i2c.h"
#include "stm32_otg.h"
#include "stm32_pwr.h"
#include "stm32_rcc.h"
#include "stm32_rng.h"
#include "stm32_rtc.h"
#include "stm32_sdio.h"
#include "stm32_spi.h"
#include "stm32_syscfg.h"
#include "stm32_tim.h"
#include "stm32_usart.h"
#include "stm32_wdg.h"
#ifdef __STM32F4xx__
#include "stm32_dma2d.h"
#include "stm32_ltdc.h"
#include "stm32_sai.h"
#endif*/

/* TODO: move these */
#define TIM2_REGION1_START          (PERIPH_BASE + 0x0)
#define TIM3_REGION1_START          (PERIPH_BASE + 0x400)
#define TIM4_REGION1_START          (PERIPH_BASE + 0x800)
#define TIM5_REGION1_START          (PERIPH_BASE + 0xc00)
#define TIM6_REGION1_START          (PERIPH_BASE + 0x1000)
#define TIM7_REGION1_START          (PERIPH_BASE + 0x1400)
#define TIM12_REGION1_START         (PERIPH_BASE + 0x1800)
#define TIM13_REGION1_START         (PERIPH_BASE + 0x1c00)
#define TIM14_REGION1_START         (PERIPH_BASE + 0x2000)
#define WWDG_BASE           (PERIPH_BASE + 0x2c00)
#define IWDG_BASE           (PERIPH_BASE + 0x3000)
#define CAN1_BASE           (PERIPH_BASE + 0x6400)
#define CAN2_BASE           (PERIPH_BASE + 0x6800)
#define DAC_BASE            (PERIPH_BASE + 0x7400)
#define TIM1_BASE           (PERIPH_BASE + 0x10000)
#define TIM8_BASE           (PERIPH_BASE + 0x10400)
#define SYSCFG_BASE         (PERIPH_BASE + 0x13800)
#define EXTI_BASE           (PERIPH_BASE + 0x13c00)
#define TIM9_BASE           (PERIPH_BASE + 0x14000)
#define TIM10_BASE          (PERIPH_BASE + 0x14400)
#define TIM11_BASE          (PERIPH_BASE + 0x14800)
#define GPIOA_BASE          (PERIPH_BASE + 0x20000)
#define GPIOB_BASE          (PERIPH_BASE + 0x20400)
#define GPIOC_BASE          (PERIPH_BASE + 0x20800)
#define GPIOD_BASE          (PERIPH_BASE + 0x20c00)
#define GPIOE_BASE          (PERIPH_BASE + 0x21000)
#define GPIOF_BASE          (PERIPH_BASE + 0x21400)
#define GPIOG_BASE          (PERIPH_BASE + 0x21800)
#define GPIOH_BASE          (PERIPH_BASE + 0x21c00)
#define GPIOI_BASE          (PERIPH_BASE + 0x22000)
#define CRC_BASE            (PERIPH_BASE + 0x23000)
#define FLASH_REG_BASE      (PERIPH_BASE + 0x23c00) /* Location of flash interface register, not flash memory itself */
#define BKPSRAM_BASE        (PERIPH_BASE + 0x24000)
#define ETH_BASE            (PERIPH_BASE + 0x28000)
#define OTG_HS_BASE         (PERIPH_BASE + 0x40000)
#define OTG_FS_BASE         (PERIPH_BASE + 0x10000000)
#define DCMI_BASE           (PERIPH_BASE + 0x10050000)
#define CRYP_BASE           (PERIPH_BASE + 0x10060000)
#define HASH_BASE           (PERIPH_BASE + 0x10060400)
#define RNG_BASE            (PERIPH_BASE + 0x10060800)
#define FSMC_CTRL_BASE      0xA0000000

#ifdef __STM32F4xx__
#define I2S2ext_BASE        (PERIPH_BASE + 0x3400)
#define I2S3ext_BASE        (PERIPH_BASE + 0x4000)
#define SAI1_BASE           (PERIPH_BASE + 0x15800)
#define LCD_TFT_BASE        (PERIPH_BASE + 0x16800)
#endif

#endif /* _DRIVERS_H */
