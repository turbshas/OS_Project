# Requires MAKEFILE_DIR and MAIN_MAKEFILE_DIR variables to be set
ifeq ($(MAKELEVEL),0)

ifeq ($(MAKECMDGOALS),)
all:
	@$(MAKE) -C $(MAIN_MAKEFILE_DIR)
else
%:
	@$(MAKE) $@ -C $(MAIN_MAKEFILE_DIR)
endif

else
SRC_FILES += $(patsubst $(CURDIR)/%,%,$(wildcard $(MAKEFILE_DIR)/*.c))
INCLUDES += -I$(MAKEFILE_DIR)
endif

