# Lower Makefiles use MAKELEVEL to make decisions, so if make is invoked from this directory, make it look like a recursive invocation
ifeq ($(MAKELEVEL),0)
MAKELEVEL := 1
endif

SRC_FILES :=

INCLUDES := -I.

MODULES :=\
	hw \
	os

include $(patsubst %,%/Makefile, $(MODULES))

# Rules for building
OBJ_FILES := $(patsubst %.c,build/%.o,$(SRC_FILES))
DEP_FILES := $(OBJ_FILES:.o=.dep)

# CC := arm-none-eabi-gcc
CC := arm-none-eabi-g++
OBJCOPY := arm-none-eabi-objcopy

LINKER_SCRIPT := build/startup.ld.preproc
ELF := build/startup.elf
BINARY := build/startup.bin

LINKER_FLAGS:=\
    -Wl,-T$(LINKER_SCRIPT) \
    --specs=nosys.specs

COMPILE_FLAGS:=\
    -O0 \
    -g \
    -Wall \
	-Wextra \
    -mcpu=cortex-m3 \
    -mfloat-abi=softfp \
    -mfpu=vfp \
    -mthumb

all: $(BINARY)

# Get make to recompile when header files are changed
-include $(DEP_FILES)

# Comment out the line below to print commands used when building
HIDE_OUTPUT := @

build/%.o: %.c
	@echo "    CC    $<"
	$(HIDE_OUTPUT)mkdir -p $(dir $@)
	$(HIDE_OUTPUT)$(CC) -c -MMD $(COMPILE_FLAGS) $(INCLUDES) $< -o $@

$(LINKER_SCRIPT): startup.ld
	@echo "    GEN   $(notdir $@)"
	$(HIDE_OUTPUT)mkdir -p $(dir $@)
	$(HIDE_OUTPUT)$(CC) -E -x c $< | grep -v "^#" > $@

$(ELF): $(LINKER_SCRIPT) $(OBJ_FILES)
	@echo "    LD    $(notdir $@)"
	$(HIDE_OUTPUT)mkdir -p $(dir $@)
	$(HIDE_OUTPUT)$(CC) $(LINKER_FLAGS) $(OBJ_FILES) -o $@

$(BINARY): $(ELF)
	@echo "    BIN   $(notdir $@)"
	$(HIDE_OUTPUT)mkdir -p $(dir $@)
	$(HIDE_OUTPUT)$(OBJCOPY) -O binary $< $@

# Rules for running and debugging
QEMU := qemu-pebble
GDB := gdb-multiarch
GDB_PORT := 63770

run: $(BINARY)
	$(QEMU) -rtc base=localtime -serial null -serial null -serial stdio -gdb tcp::$(GDB_PORT),server -machine pebble-bb2 -cpu cortex-m3 -pflash $< -S

debug: $(ELF) $(BINARY)
	$(GDB) -tui --eval-command="target remote localhost:$(GDB_PORT)" $<

clean:
	$(HIDE_OUTPUT)rm -rf build

.PHONY: all default run debug clean

