# Building and Running NyxOS

## Prerequisites

- **Cross-compiler:** `x86_64-elf-gcc` or a host GCC with `-m64` support
- **NASM:** Netwide Assembler
- **QEMU:** v7.0+ recommended
- **Python 3:** For initramfs generation
- **GNU/Linux or WSL** recommended

## Quick start

### Build the kernel
```sh
make -C kernel
```
Produces `kernel/nyx-kernel.bin`.

### Build an ISO
```sh
./tools/build.sh
```
Creates `NyxOS.iso`.

### Run in QEMU
```sh
./run.ps1              # GUI mode
./run.ps1 -Mode serial # Serial debug
./run.ps1 -Mode net    # With networking
```

## Build system

### Kernel Makefile
```sh
make -C kernel
make -C kernel clean
```

**Flags:** `-std=gnu99 -ffreestanding -Os -Wall -Wextra -m64 -mno-red-zone -mno-sse -mcmodel=large`

### User-space programs
User ELFs are built from `user/`:
```sh
make -C kernel  # also builds user/*.elf
```

### Initramfs
```sh
python3 tools/mkinitramfs.py kernel/initramfs_data.h c
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| `gcc: command not found` | Install GCC or set `CC` in makefile |
| `nasm: command not found` | `sudo apt install nasm` |
| `grub-mkrescue: not found` | `sudo apt install grub-pc-bin xorriso` |
| Triple fault on boot | Check QEMU serial output; common: wrong page tables, missing IDT, stack overflow |
