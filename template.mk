# Requires MAKEFILE_DIR and MAIN_MAKEFILE_DIR and variables to be set
ifeq ($(MAKELEVEL),0)
default: all

%:
	@$(MAKE) $(MAKEFLAGS) $@ -C $(MAIN_MAKEFILE_DIR)

else
SRC_FILES += $(wildcard $(MAKEFILE_DIR)/*.c)
INCLUDES += -I$(MAKEFILE_DIR)
endif

