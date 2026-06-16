<h1>32-bit(x86) OS 💾</h1>

### A minimal 32-bit(x86) OS written in C and ASM.


## Tools Needed
- qemu
- bochs
- make
- nasm
- grub-install
- i686-elf-gcc
- i686-elf-ld
- mkfs.fat

### To Build & Run
```sh
make drive
make all
make run/debug

```


#### I use seabios as my bios when using bochs, the default bochs's bios triple faults immediately, whenever I use it.