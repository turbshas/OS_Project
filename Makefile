# Lower Makefiles use MAKELEVEL to make decisions, so if make is invoked from this directory, make it look like a recursive invocation
ifeq ($(MAKELEVEL),0)
MAKELEVEL := 1
endif

SRC_FILES :=
SRC_FILE_EXTENSION := cpp

INCLUDES := -I.

MODULES :=\
	hw \
	os

include $(patsubst %,%/Makefile, $(MODULES))

# Rules for building
OBJ_FILES := $(patsubst %.$(SRC_FILE_EXTENSION),build/%.o,$(SRC_FILES))
DEP_FILE_EXTENSION := d
DEP_FILES := $(OBJ_FILES:.o=.$(DEP_FILE_EXTENSION))

# CC := arm-none-eabi-gcc
CC := arm-none-eabi-g++
OBJCOPY := arm-none-eabi-objcopy

LINKER_SCRIPT := build/startup.ld.preproc
ELF := build/startup.elf
BINARY := build/startup.bin

ARCH_FLAGS:=\
	-march=armv7-m \
	-mcpu=cortex-m3 \
	-mthumb

COMPILE_AND_LINK_FLAGS:=\
	$(ARCH_FLAGS) \
	-nostdlib

LINKER_FLAGS:=\
	$(COMPILE_AND_LINK_FLAGS) \
    -Wl,-T$(LINKER_SCRIPT) \
    --specs=nosys.specs \

COMPILE_FLAGS:=\
	$(COMPILE_AND_LINK_FLAGS) \
	-std=c++17 \
    -O0 \
    -g \
    -Wall \
	-Wextra \
	-fno-exceptions \
	-fno-unwind-tables \
	-fno-rtti
# stuff to disable std lib
all: $(BINARY)

# Get make to recompile when header files are changed
-include $(DEP_FILES)

# Comment out the line below to print commands used when building
HIDE_OUTPUT := @

build/%.o: %.$(SRC_FILE_EXTENSION)
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

TEST_QEMU := qemu-system-gnuarmeclipse

test_run: $(BINARY)
	$(TEST_QEMU) -serial null -serial null -serial stdio -gdb tcp::$(GDB_PORT),server -machine STM32F4-Discovery -cpu cortex-m4 -kernel $< -S

run: $(BINARY)
	$(QEMU) -rtc base=localtime -serial null -serial null -serial stdio -gdb tcp::$(GDB_PORT),server -machine pebble-bb2 -cpu cortex-m3 -pflash $< -S

debug: $(ELF) $(BINARY)
	$(GDB) -tui --eval-command="target remote localhost:$(GDB_PORT)" $<

clean:
	$(HIDE_OUTPUT)rm -rf build

readelf: $(ELF)
	arm-none-eabi-readelf $< -a

.PHONY: all default run debug clean readelf

