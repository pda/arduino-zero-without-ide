arduino-zero-without-ide
========================

Messing with an Arduino Zero sans-IDE, on a ThinkPad running Arch Linux.

Installed packages
------------------

```
arm-none-eabi-binutils
arm-none-eabi-gcc
arm-none-eabi-gdb
arm-none-eabi-newlib
```

BOSSA
-----

Atmel AVR uses [avrdude] but Atmel SAM (ARM core) uses [BOSSA].

BOSSA mainline (including Arch's `bossa-bin` AUR package) currently doesn't support Arduino Zero;

```
$ bossac --port=ttyACM0 --info
No device found on ttyACM0
```

Arduino uses a `bossac` binary built from the `arduino` branch at https://github.com/shumatech/BOSSA/tree/arduino (it took me a long time, and recompiling the `cdc_acm` kernel module with debug enabled, to figure that out).

```
$ bossac --port=ttyACM0 --info
Atmel SMART device 0x10010005 found
Device       : ATSAMD21G18A
Chip ID      : 10010005
Version      : v1.1 [Arduino:XYZ] Jun 10 2015 11:08:10
Address      : 8192
Pages        : 3968
Page Size    : 64 bytes
Total Size   : 248KB
Planes       : 1
Lock Regions : 16
Locked       : none
Security     : false
Boot Flash   : true
BOD          : true
BOR          : true
Arduino      : FAST_CHIP_ERASE
Arduino      : FAST_MULTI_PAGE_WRITE
Arduino      : CAN_CHECKSUM_MEMORY_BUFFER
```

Install BOSSA Arduino branch from source:

```
git clone --branch arduino https://github.com/shumatech/BOSSA.git ~/code/bossa
cd ~/code/bossa
make
mkdir -p ~/bin && ln -s ~/code/BOSSA/bin/bossa* ~/bin/
```

Atmel Cortex Microcontroller Software Interface Standard (CMSIS)
----------------------------------------------------------------

* ARM's [CMSIS - Cortex Microcontroller Software Interface Standard][CMSIS] is a thing.
* Atmel's [Atmel Software Framework (ASF)][ASF] [supports CMSIS][ASF-CMSIS].
* ASF (421 MB): [asf-standalone-archive-3.30.0.43.zip][ASF download]
* CMSIS (97 MB): [CMSIS-SP-00300-r4p5-00rel0/CMSIS-SP-00300-r4p5-00rel0.zip][CMSIS download]

Files that look specifically relevant:

```
sam0/utils/cmsis/samd21/include/pio/samd21g18a.h
sam0/utils/cmsis/samd21/include/samd21g18a.h
sam0/utils/cmsis/samd21/source/gcc/startup_samd21.c
sam0/utils/linker_scripts/samd21/gcc/samd21g18a_flash.ld
sam0/utils/linker_scripts/samd21/gcc/samd21g18a_sram.ld
```

Other headers etc in `sam0/utils/cmsis/samd21` could also be useful.


[ASF download]: http://www.atmel.com/images/asf-standalone-archive-3.30.0.43.zip
[ASF-CMSIS]: http://asf.atmel.com/docs/latest/cmsis.html
[ASF]: http://asf.atmel.com/docs/latest/index.html
[BOSSA]: http://www.shumatech.com/web/products/bossa
[CMSIS download]: https://silver.arm.com/download/ARM_and_AMBA_Architecture/CMSIS-SP-00300-r4p5-00rel0/CMSIS-SP-00300-r4p5-00rel0.zip
[CMSIS]: http://www.arm.com/products/processors/cortex-m/cortex-microcontroller-software-interface-standard.php
[avrdude]: http://www.nongnu.org/avrdude/
