# Requires MAIN_MAKEFILE_DIR variables to be set
ifeq ($(MAKELEVEL),0)

ifeq ($(MAKECMDGOALS),)
all:
	@$(MAKE) -C $(MAIN_MAKEFILE_DIR)
else
%:
	@$(MAKE) $@ -C $(MAIN_MAKEFILE_DIR)
endif

else
NUM_MAKEFILES = $(words $(MAKEFILE_LIST))
# This file is the last in the makefile list, the one we want is second-last.
MAKEFILE_PATH := $(word $(NUM_MAKEFILES), PushItem $(MAKEFILE_LIST))
# Strip the trailing / from the path.
MAKEFILE_DIR := $(patsubst %/,%, $(dir $(MAKEFILE_PATH)))

SEEN_FILES := $(patsubst $(CURDIR)/%,%,$(wildcard $(MAKEFILE_DIR)/*.$(SRC_FILE_EXTENSION)))
SRC_FILES += $(SEEN_FILES)
INCLUDES += -I$(MAKEFILE_DIR)

SUBMODULES := $(shell ls -d $(MAKEFILE_DIR)/*/ 2>/dev/null || true)
include $(patsubst %/, %/Makefile, $(SUBMODULES))
endif

