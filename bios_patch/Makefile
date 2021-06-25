# Makefile for DSO203 Alternate BIOS patch hooks
# Petteri Aimonen <jpa@dso.mail.kapsi.fi> 2012

# BIOS image to patch
ORIGINAL = SYS_B164.HEX

# Output image
OUTPUT = ALT_B164.HEX

# Object files
OBJS = bios_patch.o

# Linker script (choose which application position to use)
LFLAGS  = -T bios_patch.lds

# Names of the toolchain programs
CPP     = arm-none-eabi-cpp
CC      = arm-none-eabi-gcc
CP      = arm-none-eabi-objcopy
OD      = arm-none-eabi-objdump

# Processor type
CFLAGS += -mcpu=cortex-m3 -mthumb -mno-thumb-interwork

# Optimization & debug settings
CFLAGS += -fno-common -Os -g -std=gnu99 -DNDEBUG

# Compiler warnings
CFLAGS += -Wall -Werror -Wno-unused

# Default linker arguments (disables GCC-provided startup.c, creates .map file)
LFLAGS += -nostartfiles -nostdlib -Wl,-Map=build/$(NAME).map

# Directory for .o files
VPATH = build
_OBJS = $(addprefix build/,$(OBJS))

all: $(OUTPUT)

build:
	mkdir -p build

clean:
	$(RM) $(OUTPUT) build/*

check: $(OUTPUT)
	arm-none-eabi-objdump -D -m arm -M force-thumb SYS_ALT.HEX | less

$(OUTPUT): build/patched.bin
	$(CP) -I binary -O ihex --adjust-vma=0x08004000 $< $@

build/patched.bin: build/patch.bin build/orig.bin
	python2 ../tools/combine.py $^ $@

build/orig.bin: $(ORIGINAL)
	$(CP) -I ihex -O binary $< $@

build/patch.bin: build/patch.elf
	$(CP) -O binary $< $@

build/patch.elf: ${_OBJS} bios_patch.lds
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ ${_OBJS}

build/%.o: %.s | build
	$(CC) $(CFLAGS) -c -o $@ $<

deploy: $(OUTPUT)
	mount /mnt/dso
	cp $< /mnt/dso
	sync
	umount /mnt/dso