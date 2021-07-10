# Bare-Metal-Encryption

## Requirements
Thesis project 2020-2021

Topic: Bare-metal application for x86 processors on a QEMU/KVM hypervisor  
Requirements: 
The application has to encrypt/decrypt data with asymmetric encryption sent over virtio-serial and to return results over the same connection. 
The application has to implement the virtio-serial connection.

## Achievements
A bare-metal application running on the QEMU hypervisor implementing the RSA algorithm for asymmetric encryption and a serial driver for the serial connection. The virtio-serial driver was abandoned after a month of fruitless debugging.

## Documentation
The documentation can be found as a pdf in the [latest release](https://github.com/umnikos/Bare-Metal-Encryption/releases). It's written in Bulgarian because that's one of the requirements and I am too lazy to translate it. Here's "documentation lite", I guess:

### Folder structure
`docs/` - The .latex and .tex files for the documentation  
`kernel/` - The barebones assembly and C code for running anything on bare metal  
`serial/` - A regular serial driver for communication  
`virtio/` - A (currently broken) virtio-serial driver for communication  
`encryption/` - An implementation of RSA  
`builds/` - Makefiles and glue code for different combinations of the 4 folders mentioned above  
`tiny-bignum-c/` - A modified fork of [kokke/tiny-bignum-c](https://github.com/kokke/tiny-bignum-c) because I didn't know .patch files existed

### Compiling the code
The code needs to be compiled with a gcc cross-compiler. Figure 4.7 of the documentation lists the commands needed to make such a compiler, but I got them from [the OSDev wiki](https://wiki.osdev.org/GCC_Cross-Compiler) so you can just follow that. I have also experimented with using the zig compiler as an off-the-shelf substitution but I couldn't get it to link together.

## Reception
My biggest technical mistake was to choose xorshift as my pseudo-rng, compromising the security of the application while actually taking more effort to implement than just using [`RDRAND`](https://www.felixcloutier.com/x86/rdrand). My documentation also wasn't perfect, despite my best efforts. Despite that I got my A. 😄

