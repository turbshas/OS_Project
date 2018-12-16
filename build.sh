#TODO: make a better solution than this, a Makefile or something

cd ~/Pebble_Startup

INCLUDES="\
    -I./drivers \
    -I./drivers/RTC \
"

SRC_FILES="\
    startup.c \
    ./drivers/RTC/RTC.c \
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
