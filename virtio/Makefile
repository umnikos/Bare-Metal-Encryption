SHELL := /bin/bash

CC = gcc
AS = nasm
COMPILE = $(CC) -g -O2 -Wall -Wextra -c
ASSEMBLE = $(AS) -g -f elf32 -Wall -Werror

asmfiles = $(subst .asm,.asm.o,$(wildcard *.asm))
cfiles = $(subst .c,.c.o,$(wildcard *.c))
hfiles = $(wildcard *.h)

all.o: $(asmfiles) $(cfiles) $(hfiles);
	$(CC) -g -O2 -Wall -Wextra $(asmfiles) $(cfiles)

%.asm.o: %.asm
	$(ASSEMBLE) $< -o $@
%.c.o: %.c
	$(COMPILE) $< -o $@

%.asm:;
