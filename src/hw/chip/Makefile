MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR := $(patsubst %/,%, $(dir $(MAKEFILE_PATH)))
MAIN_MAKEFILE_DIR := ../../..

include $(MAKEFILE_DIR)/$(MAIN_MAKEFILE_DIR)/template.mk

ifeq ($(MAKELEVEL),1)
SUBMODULES :=\
	stm32_dma \
	stm32_exti \
	stm32_pwr \
	stm32_rcc \
	stm32_rtc \
	stm32_syscfg \
	stm32_usart

include $(patsubst %, $(MAKEFILE_DIR)/%/Makefile, $(SUBMODULES))
endif

