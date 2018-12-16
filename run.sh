#TODO: make better

cd ~/Pebble_Startup

qemu-pebble -rtc base=localtime -serial null -serial null -serial stdio -gdb tcp::63770,server -machine pebble-bb2 -cpu cortex-m3 -pflash startup.bin -S
