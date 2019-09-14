ifeq ($(MAKELEVEL),0)
MAKELEVEL := 1
endif

SRC_FILES :=

INCLUDES := -I.

MODULES :=\
	hw

include $(patsubst %,%/Makefile, $(MODULES))

# Rules for building
OBJ_FILES := $(patsubst %.c,build/%.o,$(SRC_FILES))
DEP_FILES := $(OBJ_FILES:.o=.dep)

CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy

LINKER_SCRIPT := build/startup.ld.preproc
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

all: build/startup.bin

# Get make to recompile when header files are changed
-include $(DEP_FILES)

build/%.o: %.c
	@echo "    CC    $<"
	@mkdir -p $(dir $@)
	@$(CC) -c -MMD $(COMPILE_FLAGS) $(INCLUDES) $< -o $@

$(LINKER_SCRIPT): startup.ld
	@echo "   GEN    $(notdir $@)"
	@mkdir -p $(dir $@)
	@$(CC) -E -x c $< | grep -v "^#" > $@

build/startup.elf: $(LINKER_SCRIPT) $(OBJ_FILES)
	@echo "    LD    $(notdir $@)"
	@mkdir -p $(dir $@)
	@$(CC) $(LINKER_FLAGS) $(OBJ_FILES) -o $@

build/startup.bin: build/startup.elf
	@echo "   BIN    $(notdir $@)"
	@mkdir -p $(dir $@)
	@$(OBJCOPY) -O binary $< $@

# Rules for running and debugging
QEMU := qemu-pebble
GDB := gdb-multiarch
GDB_PORT := 63770

run: build/startup.bin
	$(QEMU) -rtc base=localtime -serial null -serial null -serial stdio -gdb tcp::$(GDB_PORT),server -machine pebble-bb2 -cpu cortex-m3 -pflash $< -S

debug: build/startup.elf build/startup.bin
	$(GDB) -tui --eval-command="target remote localhost:$(GDB_PORT)" $<

clean:
	@rm -rf build

.PHONY: all default run debug clean

