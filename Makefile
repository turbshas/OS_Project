ifeq ($(MAKELEVEL),0)
MAKELEVEL := 1
endif

SRC_FILES :=

INCLUDES := -I.

MODULES :=\
	hw

include $(patsubst %, %/Makefile, $(MODULES))

OBJ_FILES := $(SRC_FILES:.c=.o)

CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
QEMU := qemu-pebble
GDB := gdb-multiarch

LINKER_FLAGS:=\
    -Wl,-Tstartup.ld.preproc \
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

all: startup.bin

# Just need the actual rules to run when source file changes, so use an empty rule for .c and .h files
%.o: %.c
	$(CC) -c $(COMPILE_FLAGS) $(INCLUDES) $^ -o $@

%.h: ;

startup.ld.preproc: startup.ld
	$(CC) -E -x c $< | grep -v "^#" > $@

startup.elf: startup.ld.preproc $(OBJ_FILES)
	$(CC) $(LINKER_FLAGS) $(OBJ_FILES) -o $@

startup.bin: startup.elf
	$(OBJCOPY) -O binary $< $@

run: startup.bin
	$(QEMU) -rtc base=localtime -serial null -serial null -serial stdio -gdb tcp::63770,server -machine pebble-bb2 -cpu cortex-m3 -pflash $< -S

debug: startup.elf startup.bin
	$(GDB) -tui --eval-command="target remote localhost:63770" $<

clean:
	@rm -rf $(OBJ_FILES)
	@rm -f startup.ld.preproc startup.elf startup.bin

.PHONY: all default run debug clean

