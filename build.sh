#TODO: make a better solution than this, a Makefile or something

cd ~/Pebble_Startup

INCLUDES="\
    -I./drivers \
    -I./drivers/ADC \
    -I./drivers/I2C \
    -I./drivers/PWR \
    -I./drivers/RCC \
    -I./drivers/RTC \
    -I./drivers/SDIO \
    -I./drivers/SPI \
    -I./drivers/USART \
"

SRC_FILES="\
    startup.c \
    ./drivers/ADC/ADC.c \
    ./drivers/I2C/I2C.c \
    ./drivers/PWR/PWR.c \
    ./drivers/RCC/RCC.c \
    ./drivers/RTC/RTC.c \
    ./drivers/SDIO/SDIO.c \
    ./drivers/SPI/SPI.c \
    ./drivers/USART/USART.c \
"

LINKER_FLAGS="\
    -Wl,-Tstartup.ld.preproc \
"

COMPILE_FLAGS="\
    -O0 \
    -g \
    -Wall \
    -mcpu=cortex-m3 \
    -mfloat-abi=softfp \
    -mfpu=vfp \
    -mthumb \
    -lc \
    --specs=nosys.specs \
"

# Apply preprocessor to our linker script
arm-none-eabi-gcc -E -x c startup.ld | grep -v "^#" > startup.ld.preproc

# Compile and link using our linker script
arm-none-eabi-gcc $COMPILE_FLAGS $INCLUDES $SRC_FILES $LINKER_FLAGS -o startup.elf

# Turn ELF into binary
arm-none-eabi-objcopy -O binary startup.elf startup.bin
