SHELL := /bin/bash

myos.iso: myos.bin
	if test -d isodir; then rm -r isodir; fi
	mkdir -p isodir/boot/grub
	cp grub.cfg isodir/boot/grub/
	cp myos.bin isodir/boot/
	grub-mkrescue -o myos.iso isodir

myos.bin: boot.o
	i686-elf-gcc -g -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o -lgcc

boot.o: boot.asm
	nasm -g -f elf32 boot.asm -o boot.o



run: myos.iso
	qemu-system-i386 -s -S -cdrom myos.iso &
	# could not get radare2 to break at a breakpoint
	# r2 -b32 -e dbg.hwbp=false -e asm.bits=32 -e bin.baddr=0x00100000 -e dbg.exe.path=myos.bin -d gdb://localhost:1234
	cat <(echo "target remote localhost:1234") - | gdb myos.bin


clean:
	if test -d isodir; then rm -r isodir; fi
	if test -f myos.iso; then rm myos.iso; fi
	if test -f myos.bin; then rm myos.bin; fi
	if test -f boot.o; then rm boot.o; fi

