# Compile
#arm-none-eabi-gcc -O0 -c -g -Wall -mcpu=cortex-m3 -mfloat-abi=softfp -mfpu=vfp -mthumb -o startup.o startup.c

arm-none-eabi-gcc -E -x c startup.ld | grep -v "^#" > startup.ld.preproc
arm-none-eabi-gcc -O0 -g -Wall -mcpu=cortex-m3 -mfloat-abi=softfp -mfpu=vfp -mthumb -Wl,-Tstartup.ld.preproc -lc --specs=nosys.specs -o startup.elf startup.c

# Apply preprocessor to our linker script
#arm-none-eabi-gcc -E -x c startup.ld | grep -v "^#" > startup.ld.preproc

# Link using our linker script
#arm-none-eabi-ld -Tstartup.ld.preproc -o startup.elf startup.o

# Turn ELF into binary
arm-none-eabi-objcopy -O binary startup.elf startup.bin
