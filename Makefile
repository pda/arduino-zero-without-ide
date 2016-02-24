PROJ_NAME = main

CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump

CFLAGS = -mcpu=cortex-m0plus -mthumb
CFLAGS += -Wall -Werror -std=c11
CFLAGS += -O0
CFLAGS += -g
CFLAGS += -ffunction-sections -fdata-sections # cargo-cult
CFLAGS += -Wl,--gc-sections -Wl,-Map=$(PROJ_NAME).map
CFLAGS += -T samd21g18a_flash.ld

CFLAGS += -I ./config
CFLAGS += -I asf/common/utils
CFLAGS += -I asf/common2/services/delay
CFLAGS += -I asf/common2/services/delay/sam0
CFLAGS += -I asf/sam0/drivers/port
CFLAGS += -I asf/sam0/drivers/sercom
CFLAGS += -I asf/sam0/drivers/sercom/spi
CFLAGS += -I asf/sam0/drivers/system
CFLAGS += -I asf/sam0/drivers/system/clock
CFLAGS += -I asf/sam0/drivers/system/clock/clock_samd21_r21_da
CFLAGS += -I asf/sam0/drivers/system/clock/clock_samd21_r21_da/module_config
CFLAGS += -I asf/sam0/drivers/system/interrupt
CFLAGS += -I asf/sam0/drivers/system/interrupt/system_interrupt_samd21
CFLAGS += -I asf/sam0/drivers/system/pinmux
CFLAGS += -I asf/sam0/drivers/system/power/power_sam_d_r
CFLAGS += -I asf/sam0/drivers/system/reset/reset_sam_d_r
CFLAGS += -I asf/sam0/utils
CFLAGS += -I asf/sam0/utils/cmsis/samd21/include
CFLAGS += -I asf/sam0/utils/cmsis/samd21/source
CFLAGS += -I asf/sam0/utils/header_files
CFLAGS += -I asf/sam0/utils/preprocessor
CFLAGS += -I asf/thirdparty/CMSIS/Include

CFLAGS += -D __SAMD21G18A__
CFLAGS += -D SYSTICK_MODE

SRCS = $(PROJ_NAME).c
SRCS += asf/common/utils/interrupt/interrupt_sam_nvic.c
SRCS += asf/common2/services/delay/sam0/systick_counter.c
SRCS += asf/sam0/drivers/port/port.c
SRCS += asf/sam0/drivers/sercom/sercom.c
SRCS += asf/sam0/drivers/sercom/spi/spi.c
SRCS += asf/sam0/drivers/system/clock/clock_samd21_r21_da/clock.c
SRCS += asf/sam0/drivers/system/clock/clock_samd21_r21_da/gclk.c
SRCS += asf/sam0/drivers/system/pinmux/pinmux.c
SRCS += asf/sam0/utils/cmsis/samd21/source/gcc/startup_samd21.c
SRCS += asf/sam0/utils/cmsis/samd21/source/system_samd21.c

.PHONY: all
all: $(PROJ_NAME).bin

$(PROJ_NAME).bin: $(PROJ_NAME).elf $(PROJ_NAME).lst
	$(OBJCOPY) $< $@ -O binary

$(PROJ_NAME).lst: $(PROJ_NAME).elf
	$(OBJDUMP) -D $^ > $@

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@


.PHONY: clean
clean:
	rm -f -- *.bin *.elf *.o *.lst

.PHONY: upload
upload:
	openocd -d2 -f openocd.cfg -c "program {{main.bin}} verify reset 0x00002000; shutdown"

.PHONY: bossa
bossa:
	bossac --port=ttyACM0 --info --erase --write --verify --reset main.bin

.PHONY: bootloader
bootloader:
	openocd -d2 -f openocd.cfg -c "init; halt; at91samd bootloader 0; program {{arduino/samd21_sam_ba.bin}} verify reset; shutdown"
