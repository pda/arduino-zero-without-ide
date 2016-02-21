PROJ_NAME = blink

CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump

CFLAGS = -mcpu=cortex-m0plus -mthumb
CFLAGS += -Wall -std=c11
CFLAGS += -O0
CFLAGS += -g
CFLAGS += -ffunction-sections -fdata-sections # cargo-cult
CFLAGS += -Wl,--gc-sections -Wl,-Map=$(PROJ_NAME).map
CFLAGS += -T samd21g18a_flash.ld

CFLAGS += -I asf/common/utils
CFLAGS += -I asf/sam0/drivers/system/pinmux
CFLAGS += -I asf/sam0/drivers/port
CFLAGS += -I asf/sam0/utils
CFLAGS += -I asf/sam0/utils/cmsis/samd21/include
CFLAGS += -I asf/sam0/utils/cmsis/samd21/source
CFLAGS += -I asf/sam0/utils/header_files
CFLAGS += -I asf/sam0/utils/preprocessor
CFLAGS += -I asf/thirdparty/CMSIS/Include

CFLAGS += -D __SAMD21G18A__

SRCS = $(PROJ_NAME).c
SRCS += asf/sam0/drivers/port/port.c
SRCS += asf/sam0/drivers/system/pinmux/pinmux.c
SRCS += asf/sam0/utils/cmsis/samd21/source/gcc/startup_samd21.c
SRCS += asf/sam0/utils/cmsis/samd21/source/system_samd21.c

.PHONY: all
all: $(PROJ_NAME).hex

$(PROJ_NAME).hex: $(PROJ_NAME).bin
	$(OBJCOPY) -I binary -O ihex $^ $@

$(PROJ_NAME).bin: $(PROJ_NAME).elf $(PROJ_NAME).lst
	$(OBJCOPY) $< $@ -O binary

$(PROJ_NAME).lst: $(PROJ_NAME).elf
	$(OBJDUMP) -D $^ > $@

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@


.PHONY: clean
clean:
	rm -f -- *.hex *.bin *.elf *.o *.lst
