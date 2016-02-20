CC=arm-none-eabi-gcc
CFLAGS=-O0 -g -mthumb -mcpu=cortex-m0plus -Wall -std=c11
CFLAGS += -I asf/sam0/utils/cmsis/samd21/include
CFLAGS += -I asf/sam0/utils/cmsis/samd21/source
CFLAGS += -I asf/thirdparty/CMSIS/Include
CFLAGS += -D __SAMD21G18A__

LD=arm-none-eabi-ld
LDFLAGS=-T samd21g18a_flash.ld

OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump

.PHONY: all
all: blink.hex

blink.hex: blink.bin
	$(OBJCOPY) -I binary -O ihex $^ $@

blink.bin: blink.elf blink.lst
	$(OBJCOPY) $< $@ -O binary

blink.lst: blink.elf
	$(OBJDUMP) -D $^ > $@

blink.elf: startup_samd21.o system_samd21.o blink.o
	$(LD) $(LDFLAGS) -o $@ $^

blink.o: blink.c
	$(CC) -c $(CFLAGS) -o $@ $^

startup_samd21.o: asf/sam0/utils/cmsis/samd21/source/gcc/startup_samd21.c
	$(CC) -c $(CFLAGS) -o $@ $^

system_samd21.o: asf/sam0/utils/cmsis/samd21/source/system_samd21.c
	$(CC) -c $(CFLAGS) -o $@ $^


.PHONY: clean
clean:
	rm -f -- *.hex *.bin *.elf *.o
