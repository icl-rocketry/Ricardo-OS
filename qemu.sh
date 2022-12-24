#!/bin/bash

# Note my setup is prolly weird as hell so ymmv
# esptool.exe might be better off as esptool.py
# Also maybe replace the paths to the .bin with whatever env you're using

if [ $1 == "link" ]
then
    esptool.exe --chip esp32 \
        merge_bin --flash_mode dio --flash_size 4MB --fill-flash-size 4MB --output build/merged.bin \
        0x1000 .pio/build/esp32dev_idf4.4/bootloader.bin \
        0x8000 .pio/build/esp32dev_idf4.4/partitions.bin \
        0x10000 .pio/build/esp32dev_idf4.4/firmware.bin

elif [ $1 == "run" ]
then
    qemu-system-xtensa -nographic \
        -machine esp32 \
        -drive file=build/merged.bin,if=mtd,format=raw
        -global driver=timer.esp32.timg,property=wdt_disable,value=true
        -drive file=build/sd_image.bin,if=sd,format=raw

elif [ $1 == "debug" ]
then
    export QEMU_XTENSA_CORE_REGS_ONLY=1
    qemu-system-xtensa -nographic -s -S \
        -machine esp32 \
        -drive file=build/merged.bin,if=mtd,format=raw
        -global driver=timer.esp32.timg,property=wdt_disable,value=true
        -drive file=build/sd_image.bin,if=sd,format=raw
elif [ $1 == "attach" ]
then
    xtensa-esp32-elf-gdb .pio/build/esp32dev_idf4.4/firmware.elf \
        -ex "target remote :1234" \
        -ex "monitor system_reset" \
        -ex "tb app_main" -ex "c"
fi

