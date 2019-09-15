MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR := $(patsubst %/,%, $(dir $(MAKEFILE_PATH)))
MAIN_MAKEFILE_DIR := ../..

include $(MAKEFILE_DIR)/$(MAIN_MAKEFILE_DIR)/template.mk

