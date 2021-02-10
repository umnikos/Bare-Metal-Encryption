SHELL := /bin/bash

home=../..
CC = i686-elf-gcc
AS = nasm
# FIXME - remove Wno-unused-parameter
COMPILE = $(CC) -g -std=gnu99 -ffreestanding -DBN_NO_ASSERT -DBN_NO_STDIO -O0 -Wall -Wextra -Werror -Wconversion -Wno-unused-parameter -c
ASSEMBLE = $(AS) -g -f elf32 -Wall -Werror

main: clean main.c.o build;
hello: clean hello.c.o build;
main-vga: clean main-vga.c.o build;
serial-test: clean serial-test.c.o build;

build: myos.iso;

myos.iso: myos.bin
	if test -d isodir; then rm -r isodir; fi
	mkdir -p isodir/boot/grub
	cp $(home)/kernel/grub.cfg isodir/boot/grub/
	cp myos.bin isodir/boot/
	grub-mkrescue -o myos.iso isodir

roots = $(home)/kernel $(home)/virtio $(home)/encryption $(home)/tiny-bignum-c $(home)/serial
asmfiles = $(foreach R,$(roots),$(patsubst $(R)/%.asm,%.asm.o,$(wildcard $(R)/*.asm)))
cfiles = $(foreach R,$(roots),$(patsubst $(R)/%.c,%.c.o,$(wildcard $(R)/*.c)))
hfiles = $(wildcard $(home)/kernel/*.h) $(wildcard $(home)/virtio/*.h)

myos.bin: $(asmfiles) $(cfiles) $(hfiles)
	$(CC) -g -T $(home)/kernel/linker.ld -o myos.bin -ffreestanding -O0 -nostdlib *.o -lgcc
%.asm.o: $(home)/kernel/%.asm
	$(ASSEMBLE) $< -o $@
%.asm.o: $(home)/encryption/%.asm
	$(ASSEMBLE) $< -o $@
%.asm.o: $(home)/virtio/%.asm
	$(ASSEMBLE) $< -o $@
%.asm.o: %.asm
	$(ASSEMBLE) $< -o $@
%.c.o: $(home)/kernel/%.c
	$(COMPILE) $< -o $@
%.c.o: $(home)/encryption/%.c
	$(COMPILE) $< -o $@
%.c.o: $(home)/serial/%.c
	$(COMPILE) $< -o $@
%.c.o: $(home)/virtio/%.c
	$(COMPILE) $< -o $@
%.c.o: $(home)/tiny-bignum-c/%.c
	$(COMPILE) -Wno-conversion $< -o $@
%.c.o: %.c
	$(COMPILE) $< -o $@

%.asm:; # silence gnu-make implicit rule and prevent circular dependency


VIRTIOSTUFF=-device virtio-serial -chardev socket,path=/tmp/foo,server,nowait,id=foo -device virtserialport,chardev=foo,name=org.fedoraproject.port.0
qemu = qemu-system-i386 -D ./qemu.log -s $(VIRTIOSTUFF) -cdrom myos.iso
socat = socat /tmp/foo -
# could not get radare2 to break at a breakpoint
# r2 -b32 -e dbg.hwbp=false -e asm.bits=32 -e bin.baddr=0x00100000 -e dbg.exe.path=myos.bin -d gdb://localhost:1234
debugger = cat <(echo -e "target remote localhost:1234") - | gdb myos.bin
runqemu: myos.iso
	$(qemu)

run: myos.iso
	$(qemu) &
	sleep 1
	$(socat)

debug: myos.iso
	$(qemu) &
	sleep 1
	$(socat) &
	$(debugger)



clean:
	if test -d isodir; then rm -r isodir; fi
	rm -f *.iso *.bin *.o

scan:
	scan-build-7 --use-cc=$(CC) make
