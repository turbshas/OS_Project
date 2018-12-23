SRC_FILES:=\
    startup.c \
    ./drivers/ADC/ADC.c \
    ./drivers/I2C/I2C.c \
    ./drivers/PWR/PWR.c \
    ./drivers/RCC/RCC.c \
    ./drivers/RTC/RTC.c \
    ./drivers/SDIO/SDIO.c \
    ./drivers/SPI/SPI.c \
    ./drivers/USART/USART.c \

INCLUDES:=\
    -I./drivers \
    -I./drivers/ADC \
    -I./drivers/I2C \
    -I./drivers/PWR \
    -I./drivers/RCC \
    -I./drivers/RTC \
    -I./drivers/SDIO \
    -I./drivers/SPI \
    -I./drivers/USART \

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

default: binary

startup_ld: startup.ld
	arm-none-eabi-gcc -E -x c startup.ld | grep -v "^#" > startup.ld.preproc

build: startup_ld
	arm-none-eabi-gcc $(COMPILE_FLAGS) $(INCLUDES) $(SRC_FILES) $(LINKER_FLAGS) -o startup.elf

binary: build startup.elf
	arm-none-eabi-objcopy -O binary startup.elf startup.bin

run: binary
	qemu-pebble -rtc base=localtime -serial null -serial null -serial stdio -gdb tcp::63770,server -machine pebble-bb2 -cpu cortex-m3 -pflash startup.bin -S

debug: binary
	gdb-multiarch -tui --eval-command="target remote localhost:63770" ./startup.elf

clean:
	rm -rf *.o
	rm -f startup.ld.preproc startup.elf startup.bin

.PHONY: default startup_ld build binary run debug clean
