SHELL := /bin/bash

CC = i686-elf-gcc
AS = nasm
COMPILE = $(CC) -g -std=gnu99 -ffreestanding -O2 -Wall -Wextra -c
ASSEMBLE = $(AS) -g -f elf32

all: clean myos.iso

myos.iso: myos.bin
	if test -d isodir; then rm -r isodir; fi
	mkdir -p isodir/boot/grub
	cp grub.cfg isodir/boot/grub/
	cp myos.bin isodir/boot/
	grub-mkrescue -o myos.iso isodir

asmfiles = $(subst .asm,.asm.o,$(wildcard *.asm))
cfiles = $(subst .c,.c.o,$(wildcard *.c))

myos.bin: $(asmfiles) $(cfiles)
	$(CC) -g -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib *.o -lgcc
%.asm.o: %.asm
	$(ASSEMBLE) $< -o $@
%.c.o: %.c
	$(COMPILE) $< -o $@

%.asm:; # silence gnu-make implicit rule and prevent circular dependency



run: myos.iso
	# qemu-system-i386 -s -S -cdrom myos.iso &
	qemu-system-i386 -d cpu_reset -D ./qemu.log -s -device virtio-serial -chardev socket,path=/tmp/foo,server,nowait,id=foo -device virtserialport,chardev=foo,name=org.fedoraproject.port.0 -cdrom myos.iso &
	socat /tmp/foo - &

	# could not get radare2 to break at a breakpoint
	# r2 -b32 -e dbg.hwbp=false -e asm.bits=32 -e bin.baddr=0x00100000 -e dbg.exe.path=myos.bin -d gdb://localhost:1234

debug: run
	cat <(echo -e "target remote localhost:1234") - | gdb myos.bin



clean:
	if test -d isodir; then rm -r isodir; fi
	rm -f *.iso
	rm -f *.bin
	rm -f *.o

