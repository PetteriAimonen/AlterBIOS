# Makefile for DSO203 Alternate BIOS
# Petteri Aimonen <jpa@dso.mail.kapsi.fi> 2012

# Name of the target application
NAME = ALTBIOS

# Names of the object files (add all .c files you want to include)
OBJS = jump_table.o ff.o fatfs_glue.o BIOS.o alterbios.o

# Version number from GIT
COMMITID := $(shell git describe --always || echo unknown)

# Names of all header files included in the API
API_HEADERS = fatfs/ff.h

# Names of headers needed by API, but not exported from bios
DEP_HEADERS = fatfs/ffconf.h fatfs/integer.h

# Linker script (choose which application position to use)
LFLAGS  = -L linker_scripts -T shadow_bios.lds

# Any libraries to include
LIBS = 

# Include directories for .h files
INCLUDES = -I stm32_headers
CFLAGS += $(INCLUDES)

# Names of the toolchain programs
CPP     = arm-none-eabi-cpp
CC      = arm-none-eabi-gcc
CP      = arm-none-eabi-objcopy
OD      = arm-none-eabi-objdump

ifdef windir
RM = del
endif

# Processor type
CFLAGS += -mcpu=cortex-m3 -mthumb -mno-thumb-interwork

# Optimization & debug settings
CFLAGS += -fno-common -Os -g -std=gnu99 -DNDEBUG -DCOMMITID=\"$(COMMITID)\"

# Compiler warnings
CFLAGS += -Wall -Werror -Wno-unused

# Default linker arguments (disables GCC-provided startup.c, creates .map file)
LFLAGS += -nostartfiles -nostdlib -Wl,-Map=build/$(NAME).map

# Stuff to ignore when parsing header files
CPPFLAGS = -D'__extension__=' -D'__attribute__(x)=' -D'__builtin_va_list=int'
CPPFLAGS += $(INCLUDES)

# Directory for .o files
VPATH = build
_OBJS = $(addprefix build/,$(OBJS))

all: $(NAME).HEX

build:
	mkdir -p build

clean:
	$(RM) $(NAME).HEX build/*

$(NAME).HEX: build/$(NAME).elf
	$(CP) -O ihex $< $@

build/$(NAME).elf: ${_OBJS} linker_scripts/*.lds
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ ${_OBJS} ${LIBS}

build/ff.o: fatfs/ff.c fatfs/*.h
	$(CC) $(CFLAGS) -Os -fno-strict-aliasing -c -o $@ $<

build/BIOS.o: DS203/BIOS.S
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.o: %.c *.h | build
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.o: %.s | build
	$(CC) $(CFLAGS) -c -o $@ $<

deploy: $(NAME).HEX
	mount /mnt/dso
	cp $< /mnt/dso
	sync
	umount /mnt/dso

# This rule is not invoked automatically, as it is only needed when API is
# extended with new functions.

collect: $(API_HEADERS:.h=.collect)

%.collect: %.h
	$(CPP) $(CPPFLAGS) $< | python ../tools/collect_functions.py list_of_functions.json

jump_table.s: list_of_functions.json
	python ../tools/make_jump_table.py $< > $@

# These rules update the api export files.

api: build/alterbios.s build/alterbios.h $(API_HEADERS) $(DEP_HEADERS)
	cp --parents $^ ../api

build/alterbios.s: list_of_functions.json
	python ../tools/make_export.py $< > $@

build/alterbios.h: list_of_functions.json
	python ../tools/make_header.py $< > $@