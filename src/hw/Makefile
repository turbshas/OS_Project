MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR := $(patsubst %/,%, $(dir $(MAKEFILE_PATH)))
MAIN_MAKEFILE_DIR := ../..

include $(MAKEFILE_DIR)/$(MAIN_MAKEFILE_DIR)/template.mk

ifeq ($(MAKELEVEL),1)
SUBMODULES :=\
	chip \
	cpu \
	drivers

include $(patsubst %, $(MAKEFILE_DIR)/%/Makefile, $(SUBMODULES))
endif

