CURR_MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
INCLUDES += -I$(CURR_MAKEFILE_DIR)

define MAKEFILE_INCLUDES
include $(CURR_MAKEFILE_DIR)stm32_dma/Makefile
include $(CURR_MAKEFILE_DIR)stm32_pwr/Makefile
include $(CURR_MAKEFILE_DIR)stm32_rcc/Makefile
include $(CURR_MAKEFILE_DIR)stm32_rtc/Makefile
include $(CURR_MAKEFILE_DIR)stm32_usart/Makefile
endef

$(eval $(MAKEFILE_INCLUDES))

