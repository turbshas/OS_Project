#TODO: make better

cd ~/Pebble_Startup

gdb-multiarch -tui --eval-command="target remote localhost:63770" ./startup.elf
