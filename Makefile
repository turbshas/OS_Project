SRC_FILES := startup.c

INCLUDES :=

include hw/Makefile

LINKER_FLAGS:=\
    -Wl,-Tstartup.ld.preproc

COMPILE_FLAGS:=\
    -O0 \
    -g \
    -Wall \
    -mcpu=cortex-m3 \
    -mfloat-abi=softfp \
    -mfpu=vfp \
    -mthumb \
    --specs=nosys.specs

all: startup.bin

%.c: ;

startup.ld.preproc: startup.ld
	arm-none-eabi-gcc -E -x c $< | grep -v "^#" > $@

startup.elf: startup.ld.preproc $(SRC_FILES)
	arm-none-eabi-gcc $(COMPILE_FLAGS) $(INCLUDES) $(SRC_FILES) $(LINKER_FLAGS) -o $@

startup.bin: startup.elf
	arm-none-eabi-objcopy -O binary $< $@

run: startup.bin
	qemu-pebble -rtc base=localtime -serial null -serial null -serial stdio -gdb tcp::63770,server -machine pebble-bb2 -cpu cortex-m3 -pflash $< -S

debug: startup.elf startup.bin
	gdb-multiarch -tui --eval-command="target remote localhost:63770" $<

clean:
	rm -rf *.o
	rm -f startup.ld.preproc startup.elf startup.bin

.PHONY: default run debug clean
