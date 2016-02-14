CC=arm-none-eabi-gcc
CFLAGS=-O0 -g -mthumb -mcpu=cortex-m0plus -Wall -std=c11 -nostdlib -nostartfiles
LD=arm-none-eabi-ld
LDFLAGS=
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump

blink.hex: blink.bin
	$(OBJCOPY) -I binary -O ihex blink.bin blink.hex

blink.bin: blink.elf blink.list
	$(OBJCOPY) blink.elf blink.bin -O binary

blink.list: blink.elf
	$(OBJDUMP) -D blink.elf > blink.lst

blink.elf: blink.o
	$(LD) $(LDFLAGS) -o blink.elf blink.o

blink.o: blink.c
	$(CC) -c $(CFLAGS) -o blink.o blink.c

.PHONY: clean
clean:
	rm -f -- blink blink.bin blink.elf blink.o
