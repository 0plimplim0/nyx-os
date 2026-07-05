# Architecture

NyxOS is a monolithic kernel with loadable modules, running in long mode (x86_64) with 4-level paging.

## Overview

```
+--------------------------------------------------------+
|                    Userspace (Ring 3)                   |
|  init.elf  spin.elf  fdleak.elf  (future: user apps)   |
+--------------------------------------------------------+
|              System Call Interface (int 0x80)           |
+--------------------------------------------------------+
|                    Kernel (Ring 0)                      |
|  +----------+ +---------+ +--------+ +-----------+     |
|  | Scheduler | | Memory  | | VFS    | | Networking |   |
|  |  + IPC    | | Manager | | + EXT2 | | TCP/IP     |   |
|  +----------+ +---------+ +--------+ +-----------+     |
|  +---------+ +--------+ +---------+ +-----------+      |
|  | Drivers | | GUI    | | ELF     | | Initramfs |      |
|  |(kbd,mouse| | Compo- | | Loader  | |           |     |
|  | SB16,    | | sitor  | |         | |           |     |
|  | RTL8139) | |        | |         | |           |     |
|  +---------+ +--------+ +---------+ +-----------+      |
+--------------------------------------------------------+
|                 Hardware Abstraction Layer              |
+--------------------------------------------------------+
|                      x86_64 Hardware                    |
|  CPU  | APIC | I/O APIC | PIT | PCI | ATA | VBE | PS2 |
+--------------------------------------------------------+
```

## Kernel structure

All kernel source lives in `kernel/`. Key files:

| File | Purpose |
|------|---------|
| `kernel.c` | Main entry, shell, command handlers |
| `kernel.h` | Typedefs, constants, extern declarations |
| `boot.asm` | Multiboot header, GDT, long mode entry |
| `memory.c` | Bitmap physical page allocator |
| `heap.c` | 16 MB kernel heap (kmalloc/kfree) |
| `paging.c` | 4-level paging, demand paging, COW |
| `process.c` | Process table, scheduler, background tasks |
| `switch.asm` | Context switch assembly |
| `vfs.c` | Ramdisk VFS + mount table |
| `ext2.c` | EXT2 filesystem driver |
| `compositor.c` | Window compositor (32 windows, z-order) |
| `tcp.c` | Full TCP state machine |
| `elf.c` | ELF64 loader |
| `apic.c` | Local APIC + I/O APIC |
| `smp.c` | Multi-core bringup |
| `rtl8139.c` | RTL8139 NIC driver |

## Higher-half kernel

The kernel is mapped at the higher half using **PML4[511]** (the last entry). The user half occupies PML4[0..510]. The kernel page tables mirror the identity mapping (low 64 MB of physical memory accessible through the higher-half alias). This provides page-table isolation: user page directories lack the identity mapping, so a user process cannot access kernel memory.

## Key constants

- `KERNEL_BASE = 0xFFFF800000000000`
- `KERNEL_HEAP_SIZE = 16 MB`
- `MAX_PROCESSES = 64`
- `PAGE_SIZE = 4096`
- `MAX_CPUS = 8`
