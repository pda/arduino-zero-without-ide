arduino-zero-without-ide
========================

Messing with an Arduino Zero board without using the Arduino IDE, from a ThinkPad running Arch Linux.

Installed packages
------------------

```
arm-none-eabi-binutils
arm-none-eabi-gcc
arm-none-eabi-gdb
arm-none-eabi-newlib
gdb
make
openocd
```

BOSSA
-----

Atmel SAM (ARM core) can be programmed using [BOSSA]; the equivalent of programming an Atmel AVR using [avrdude]. On the Arduino Zero, the "native USB" port connects directly to the SAMD21 MCU (PA24,25; SERCOM 3) and is suitable for BOSSA.

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

Writing program to flash:

```
$ bossac -p ttyACM0 -e -w blink.bin -R
Atmel SMART device 0x10010005 found
Erase flash
done in 0.824 seconds

Write 2916 bytes to flash (46 pages)
[==============================] 100% (46/46 pages)
done in 0.025 seconds
CPU reset.
```

BOSSA only works on the native USB port, and only when in the bootloader by double-clicking the reset button (<= 500ms) or triggering a reset via the serial port. This means manual button presses to upload code, and swapping plugs to debug. Using OpenOCD on the Programming/Debug port is probably a better option.


OpenOCD
-------

[OpenOCD] is a software interface to On Chip Debuggers including the Atmel EDBG chip on the Arduino Zero's programming/debug USB port. It can be used instead of BOSSA for flashing the device, and additionally for remote debugging with GDB etc.

To give non-root access to the `/dev/ttyACM0` interface, OpenOCD ships with an example udev configuration at `/usr/share/openocd/contrib/99-openocd.rules`. There's a stripped down version in this repo, using the `uucp` group instead of `plugdev`, and only for the Atmel EDBG device. Drop it into `/etc/udev/rules.d/` or just run `openocd` as root if you really want.

As with BOSSA, Arduino has forked OpenOCD at https://github.com/arduino/OpenOCD/tree/arduino and the original does not work with Arduino Zero. The official branch/package may partially work, but the program generally wont run after programming. The difference seems to be in https://github.com/arduino/OpenOCD/commit/d4b767947e867989e461a45626c17108dcb73f61 ... I modified `PKGBUILD` from https://aur.archlinux.org/packages/openocd-git/ to use the `arduino` repo/branch, and that worked better.

The local `openocd.cfg` is based on [`arduino_zero.cfg`][OpenOCD config].

```
$ openocd
Open On-Chip Debugger 0.9.0 (2015-05-19-13:50)
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
Info : only one transport option; autoselect 'swd'
adapter speed: 500 kHz
adapter_nsrst_delay: 100
cortex_m reset_config sysresetreq
Info : CMSIS-DAP: SWD  Supported
Info : CMSIS-DAP: Interface Initialised (SWD)
Info : CMSIS-DAP: FW Version = 02.01.0157
Info : SWCLK/TCK = 1 SWDIO/TMS = 1 TDI = 1 TDO = 1 nTRST = 0 nRESET = 1
Info : CMSIS-DAP: Interface ready
Info : clock speed 500 kHz
Info : SWD IDCODE 0x0bc11477
Info : at91samd21g18.cpu: hardware has 4 breakpoints, 2 watchpoints
```

```
$ telnet 0 4444
Trying 0.0.0.0...
Connected to 0.
Escape character is '^]'.
Open On-Chip Debugger
> halt
target state: halted
target halted due to debug-request, current mode: Thread
xPSR: 0x01000000 pc: 0x00002244 msp: 0x20002418
> reset
> exit
Connection closed by foreign host.
```

```
$ arm-none-eabi-gdb -ex 'target remote localhost:3333' blink.elf
Reading symbols from blink.elf...done.
Remote debugging using localhost:3333
>>>
```



Atmel ASF and ARM CMSIS
-----------------------

* ARM's [CMSIS - Cortex Microcontroller Software Interface Standard][CMSIS] is a thing.
* Atmel's [Atmel Software Framework (ASF)][ASF] [supports CMSIS][ASF-CMSIS].
* ASF (421 MB): [asf-standalone-archive-3.30.0.43.zip][ASF download]
* CMSIS (97 MB): [CMSIS-SP-00300-r4p5-00rel0/CMSIS-SP-00300-r4p5-00rel0.zip][CMSIS download]

ASF seems more useful for an Atmel microcontroller, and ships with a vendored copy of CMSIS v4.00.

Run `./extract-asf.sh` to download (421 MB) and extract (1.5 GB) the relevant parts of ASF, excluding components for non-ARM architectures (avr32, mega, xmega).


Hardware notes
--------------

Arduino Zero

```
A0: PA02
A1: PB08
A2: PB09
A3: PA04
A4: PA05
A5: PB02
D0: PA11   SERCOM0 PAD3 mux C (SPI: unused)
D1: PA10   SERCOM0 PAD2 mux C (SPI: MISO)
D2: PA14
D3: PA09   SERCOM0 PAD1 mux C (SPI: SCK)
D4: PA08   SERCOM0 PAD0 mux C (SPI: MOSI)
D5: PA15
D6: PA20
D7: PA21
D8: PA06
D9: PA07
D10: PA18
D11: PA16
D12: PA19
D13: PA17  (LED)
```

Sparkfun SAMD21

  * Blue LED ("pin 13") is `PA17`
  * `TX_LED` (green) is `PA27`
  * `RX_LED` (yellow) is `PB03`

2.2" TFT SPA 240x320 generic board I have, probably ILI9341 or similar:

```
1: VCC
2: GND
3: CS        PA15 (D5)
4: RESET     PA21 (D7)
5: DC/RS     PA14 (D2)
6: SDI/MOSI  PA08 (D4)
7: SCK       PA09 (D3)
8: LED       PA20 (D6)
9: SDO/MISO  PA10 (D1)
```


[ASF download]: http://www.atmel.com/images/asf-standalone-archive-3.30.0.43.zip
[ASF-CMSIS]: http://asf.atmel.com/docs/latest/cmsis.html
[ASF]: http://asf.atmel.com/docs/latest/index.html
[BOSSA]: http://www.shumatech.com/web/products/bossa
[CMSIS download]: https://silver.arm.com/download/ARM_and_AMBA_Architecture/CMSIS-SP-00300-r4p5-00rel0/CMSIS-SP-00300-r4p5-00rel0.zip
[CMSIS]: http://www.arm.com/products/processors/cortex-m/cortex-microcontroller-software-interface-standard.php
[OpenOCD config]: https://github.com/arduino/OpenOCD/blob/arduino/tcl/board/arduino_zero.cfg
[OpenOCD]: http://openocd.org/
[avrdude]: http://www.nongnu.org/avrdude/
