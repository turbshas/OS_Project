CURR_MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

INCLUDES += -I$(CURR_MAKEFILE_DIR)

define MAKEFILE_INCLUDES
include $(CURR_MAKEFILE_DIR)chip/Makefile
include $(CURR_MAKEFILE_DIR)drivers/Makefile
endef

$(eval $(MAKEFILE_INCLUDES))

