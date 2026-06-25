<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=0:0a0a0a,50:1a1a1a,100:2d2d2d&height=140&section=header&text=NyxOS&fontSize=52&fontColor=00ff9d&animation=fadeIn&fontAlignY=55" />
</div>

<div align="center">
  <strong>Custom 32-bit x86 kernel · C and Assembly · General-purpose OS</strong>
  <br/><br/>
  <!-- Badges -->
  <a href="https://github.com/kazah-png/nyx-os/releases">
    <img src="https://img.shields.io/github/v/release/kazah-png/nyx-os?style=flat&color=00ff9d&label=release" />
  </a>
  <img src="https://img.shields.io/badge/kernel-80%20KB-00ff9d?style=flat" />
  <img src="https://img.shields.io/badge/arch-i686-00ff9d?style=flat" />
  <img src="https://img.shields.io/badge/status-v3.1.0-00ff9d?style=flat" />
  <img src="https://img.shields.io/badge/TCP-yes-00ff9d?style=flat" />
  <img src="https://img.shields.io/badge/GUI-window%20compositor-00ff9d?style=flat" />
  <a href="https://github.com/kazah-png/nyx-os/issues/1">
    <img src="https://img.shields.io/badge/status%20report-View-0d1117?style=flat&logo=github" />
  </a>
  <a href="https://dsc.gg/nyxos">
    <img src="https://img.shields.io/badge/Discord-NyxOS-5865F2?style=flat&logo=discord&logoColor=white" />
  </a>
</div>

---

## NyxOS — Terminal Preview

```
nyx:root$ nyxfetch

______          \'/
      .-'` .    `'-.    -= * =-
    .'  '    .---.  '.    /|\
   /  '    .'     `'. \
  ;  '    /          \|
 :  '  _ ;            `
;  :  /(\ \
|  .       '.
|  ' /     --'
|  .   '.__\
;  :       /
 ;  .     |            ,
  ;  .    \           /|
   \  .    '.       .'/
    '.  '  . `'---'`. `'
      `'-..._____.-'
    N Y X O S
    N I G H T F A L L
  -------------------------------------
  Kernel:     NyxOS 3.1.0 (Nightfall)
  Arch:       x86 (32-bit)
  Memory:     256 MB total, 240 MB free
  Heap:       16384 KB
  Paging:     Enabled
  Uptime:     42 ticks
  -------------------------------------
```

---

## About

**NyxOS** is a from-scratch 32-bit x86 kernel built as a general-purpose OS for low-level systems programming. It boots via Multiboot (GRUB-compatible), runs in protected mode with paging, and provides a clean foundation for kernel development.

The project implements core kernel primitives, a custom network stack (RTL8139 NIC + ARP/IP/UDP/ICMP + DHCP), and a ramdisk filesystem — all written in C and x86 Assembly with no external libraries.

---

## Tech stack

**Languages**

![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)
![Assembly](https://img.shields.io/badge/x86%20Assembly-6E4C13?style=flat&logo=assemblyscript&logoColor=white)
![Make](https://img.shields.io/badge/Make-003545?style=flat&logo=gnu&logoColor=white)

**Toolchain**

![GCC](https://img.shields.io/badge/GCC-FFD700?style=flat&logo=gcc&logoColor=black)
![NASM](https://img.shields.io/badge/NASM-009A9E?style=flat)
![LD](https://img.shields.io/badge/LD-00599C?style=flat)
![QEMU](https://img.shields.io/badge/QEMU-FF6600?style=flat&logo=qemu&logoColor=white)

**Kernel primitives**

![GDT/IDT](https://img.shields.io/badge/GDT%2FIDT-4B8BBE?style=flat)
![Paging](https://img.shields.io/badge/Paging-007ACC?style=flat)
![Syscalls](https://img.shields.io/badge/Syscalls-3CB371?style=flat)
![VFS](https://img.shields.io/badge/VFS-FF6347?style=flat)

**Network stack**

![Ethernet](https://img.shields.io/badge/Ethernet-2496ED?style=flat)
![ARP](https://img.shields.io/badge/ARP-00ADD8?style=flat)
![IP](https://img.shields.io/badge/IP-00599C?style=flat)
![UDP](https://img.shields.io/badge/UDP-512BD4?style=flat)
![ICMP](https://img.shields.io/badge/ICMP-FF6600?style=flat)
![DHCP](https://img.shields.io/badge/DHCP-00ff9d?style=flat)



---

## Shell session preview

```
nyx:root$ help
Available commands:
  help           - Show this help
  version        - Show kernel version
  clear          - Clear the screen
  nyxfetch       - Show system info with ASCII logo
  echo           - Print a line of text
  ls             - List directory contents
  cd             - Change directory
  pwd            - Print working directory
  cat            - Display file contents
  touch          - Create empty file
  mkdir          - Create directory
  rm             - Remove file or directory
  cp             - Copy file
  mv             - Move/rename file
  which          - Show path of a command
  head           - Show first lines of a file
  tail           - Show last lines of a file
  grep           - Search file contents
  sort           - Sort lines of a file
  wc             - Count lines/words/chars
  tree           - Show filesystem tree
  find           - Find files by name
  write          - Write text to file
  history        - Show command history
  ps             - List processes
  mem            - Show memory usage
  ifconfig       - Show network interfaces
  dhcp           - Request IP via DHCP
  ping           - Ping a host
  ...

nyx:root$ echo Hello, NyxOS!
Hello, NyxOS!

nyx:root$ echo test > /readme.txt

nyx:root$ cat /readme.txt
test

nyx:root$ ls /
bin/   dev/   etc/   home/  mnt/   root/  tmp/   usr/   var/

nyx:root$ uname
NyxOS 2.0.0 (Clean Slate) i686

nyx:root$ mem
Physical memory: 256 MB total, 252 MB free
Heap size: 1024 KB

nyx:root$ ps
PID  PPID STATE NAME
1    0    1     init

nyx:root$ history
  1  help
  2  echo Hello, NyxOS!
  3  echo test > /readme.txt
  4  cat /readme.txt
  5  ls /
  6  uname
  7  mem
  8  ps
  9  history

nyx:root$ export NAME=NyxOS

nyx:root$ echo $NAME
NyxOS

nyx:root$ touch /hello.txt && write /hello.txt Hello World

nyx:root$ sort /hello.txt
Hello World

nyx:root$ ifconfig
lo:   IP 127.0.0.1    MAC 00:00:00:00:00:00   MTU 65536
eth0: IP 10.0.2.15    MAC 52:54:00:12:34:56   MTU 1500

nyx:root$ diff /a.txt /b.txt
< line1 from a
> line1 from b
---
1 line(s) differ

nyx:root$ beep 440 500
Beep: 440 Hz for 500 ms

nyx:root$ play
Playing melody...
Done.
```

---

## Features

### Boot & initialization
- Multiboot-compliant (GRUB-ready)
- Protected mode 32-bit with GDT setup (code/data segments)
- Paging with identity mapping (64 MB, 16 page tables)
- Full IDT with exception handlers (0-31) and IRQ remapping (32-47)
- PIT timer at 1000 Hz (interrupt-driven)
- PS/2 keyboard driver (US and ES layouts + AltGr)
- PS/2 mouse driver (IRQ12, 3-byte packets, absolute positioning)
- PC speaker driver (PIT channel 2, square wave, beep/melody)
- Real-time clock (RTC) driver — CMOS RTC via ports 0x70/0x71, binary/24h init

### Memory management
- Bitmap-based physical page allocator (supports up to 512 MB)
- Kernel heap (`kmalloc`/`kfree`) with first-fit + block splitting + coalescing (16 MB heap)
- Identity-mapped page tables (64 MB)
- Per-process page directories (user PD clones kernel PD with supervisor-only PTEs)

### Process management
- Static process table (up to 512 processes)
- PID/PPID tracking, process states
- Preemptive multitasking via IRQ scheduler tick (1000 Hz)
- Context switching (`switch_context`/`create_task_stack` assembly)
- Background task callbacks for periodic work

### ELF userspace & syscalls (v3.0.0+)
- **ELF32 loader** — validates, parses PT_LOAD segments, maps pages per-process
- **Initramfs** — embedded cpio archive with ELF binaries (init.elf, hello.elf)
- **9 syscalls** via `int 0x80`: `exit`, `write`, `print`, `open`, `read`, `close`, `getpid`, `sbrk`, `exec`
- **C runtime** — minimal libc with `printf`, `sprintf`, `snprintf`, `malloc`, `free`, string/memory functions
- **Auto-boot init** — kernel loads and executes `/init.elf` from initramfs at startup
- **Ring 3 execution** — user processes run in ring 3, I/O ports denied via TSS I/O map
- **sbrk heap** — per-process heap via page allocation in user page directory

### Shell & commands
Built-in command interpreter with **40+ commands**:

| Category | Commands |
|----------|----------|
| **System** | `help`, `clear`, `nyxfetch`, `uname`, `date`, `version`, `reboot`, `crash` |
| **Files** | `ls`, `cd`, `pwd`, `cat`, `touch`, `mkdir`, `rm`, `cp`, `mv`, `head`, `tail`, `grep`, `sort`, `wc`, `find`, `tree`, `write`, `which`, `diff` |
| **Process** | `ps`, `kill`, `mem`, `exec` |
| **Network** | `ifconfig`, `dhcp`, `ping`, `setip`, `tcptest` |
| **Graphics** | `mode`, `gui`, `fonttest`, `desktop` |
| **Sound** | `beep`, `play`, `sb16play` |
| **Misc** | `echo`, `env`, `export`, `history`, `hexdump`, `layout`, `doom` |

**Shell features:**
- Tab completion for command names
- Environment variable expansion (`$VARNAME`)
- Command history (last 10, duplicates filtered)
- `echo text > file` redirection support
- Pipe `\|` support (`cmd1 \| cmd2` with temp file)

### Network stack (real)
- **RTL8139 NIC driver** — PCI detection, I/O BAR, MMIO, TX/RX ring buffers, link detection, CONFIG1 fix
- **ARP** — Cache with request/reply, static entries, periodic cleanup
- **IPv4** — Send/receive with header checksum, local delivery
- **UDP** — Raw datagram send, port-based listener registration
- **ICMP** — Echo request/reply (ping)
- **DHCP** — Full client (DISCOVER → OFFER → REQUEST → ACK), auto-configures IP/netmask/gateway
- **TCP** — Full connection state machine (CLOSED, SYN_SENT, ESTABLISHED, FIN_WAIT, CLOSE_WAIT, TIME_WAIT), 8 concurrent connections, HTTP GET support
- **Interface** — `ifconfig` for status, static IP via `setip` or DHCP-assigned

### GUI subsystem (v2.2.0+)
- **Bochs VBE framebuffer** — Up to 1024x768x32, LFB at 0xE0000000
- **Auto-boot desktop** — NyxOS Desktop launches automatically at startup
- **Framebuffer abstraction** — `put_pixel`, `fill_rect`, `blit`, `fb_rgb`
- **PS/2 mouse** — IRQ12-driven, 3-byte packet decode, absolute cursor positioning
- **VGA 8x16 bitmap font** — Full 256-glyph set from Linux kernel font data
- **Window compositor** — 32 windows, z-ordering, title bars (min/max/close), drag-to-move, resize, 4 workspaces
- **Taskbar** — Running app buttons, Start menu (12 items), clock display
- **Desktop icons** — Files, Terminal, DOOM, Settings, About, Paint
- **Terminal emulator window** — 2000-line scrollback, Tab completion, full command execution
- **File Manager window** — VFS directory browsing, click navigation, file preview
- **GUI paint demo** — 6-color mouse-driven drawing with Bresenham lines
- **PC speaker** — PIT channel 2 tone generation, musical note definitions

### EXT2 filesystem (v2.3.0)
- **VFS mount layer** — `vfs_mount()` with filesystem driver dispatch
- **Auto-mount** — EXT2 partitions auto-detected and mounted at `/mnt` on boot
- **Standard commands** — `ls /mnt`, `cd /mnt`, `cat /mnt/...` all work transparently
- **Block group support** — Multiple block groups, indirect/double-indirect blocks
- **Block sizes** — Supports 1024, 2048, and 4096 byte blocks
- **Mount command** — Manual mount via `mount [drive] [part_lba]`

---

## Project structure

```
nyx-os/
├── kernel/
│   ├── boot.asm          # Multiboot header, entry point
│   ├── kernel.c          # Main kernel, shell (40+ commands), desktop launch
│   ├── kernel.h          # Core header (types, structs, inline funcs)
│   ├── gdt.c / gdt_flush.asm / idt.c / idt_load.asm
│   ├── isr.c / isr_stubs.asm / irq.c
│   ├── memory.c          # Physical memory manager (bitmap allocator)
│   ├── heap.c            # 16 MB kernel heap allocator
│   ├── paging.c          # Page tables, virtual memory
│   ├── process.c         # Process management + background tasks
│   ├── switch.asm        # Context switch assembly
│   ├── syscall.c         # System calls (9 handlers via int 0x80)
│   ├── vfs.c             # Ramdisk VFS + mount table + pipe
│   ├── ext2.c / ext2.h   # EXT2 filesystem driver (read/write)
│   ├── ata.c / ata.h     # ATA/IDE PIO disk driver (read/write)
│   ├── dhcp.c            # DHCP client
│   ├── net.c / tcp.c / tcp.h / udp.c / ip.c / ethernet.c
│   ├── arp.c / icmp.c / rtl8139.c
│   ├── timer.c           # PIT timer (1000 Hz, interrupt-driven)
│   ├── keyboard.c        # PS/2 driver (US/ES layouts, AltGr)
│   ├── screen.c          # VGA text mode (80x25) + putchar hook
│   ├── serial.c          # COM1 debug stub
│   ├── rtc.c             # CMOS RTC driver
│   ├── vbe.c             # Bochs VBE framebuffer driver
│   ├── fb.c              # Framebuffer abstraction
│   ├── mouse.c           # PS/2 mouse driver (IRQ12)
│   ├── gui.c             # GUI paint demo with mouse
│   ├── font.c / font.h   # VGA 8x16 bitmap font (256 glyphs)
│   ├── compositor.c / compositor.h  # Window compositor (32 windows)
│   ├── terminal_win.c / terminal_win.h  # Terminal emulator window
│   ├── fileman_win.c / fileman_win.h    # File Manager window
│   ├── speaker.c / speaker.h    # PC speaker driver
│   ├── sb16.c / sb16.h          # Sound Blaster 16 driver (DMA/IRQ)
│   ├── elf.c / elf.h     # ELF32 loader for userspace binaries
│   ├── initramfs.c / initramfs.h  # Initramfs cpio parser
│   ├── initramfs_data.h  # Generated embedded initramfs archive
│   ├── vga_graphics.c    # VGA mode 13h (DOOM)
│   ├── doom_nyxos.c / doom_nyxos_sound.c  # DOOM generic port
│   └── doom_src/         # DOOM engine source
├── user/
│   ├── crt0.asm          # CRT0 for userspace ELF binaries
│   ├── syscall.h         # Syscall inline wrappers
│   ├── libc.h / libc.c   # Minimal C library (printf, malloc, string, stdio, stdlib)
│   ├── init.c            # Init program (first userspace process)
│   ├── hello.asm         # Test ELF binary
│   └── makefile          # User-space build rules (included by kernel/Makefile)
├── tools/
│   ├── build.sh          # ISO builder (grub-mkrescue)
│   ├── mkinitramfs.py    # Initramfs cpio generation script
│   ├── qemu_launch.ps1   # Windows QEMU launcher
│   └── qemu_launch.sh    # Linux QEMU launcher
├── build.ps1             # Windows build script
├── run.ps1               # Windows QEMU launcher (gui/serial/net)
├── AGENTS.md             # Agent context for AI-assisted development
├── Makefile              # Top-level build
└── README.md
```

---

## Build & run

### Prerequisites

```
i686-elf-gcc / i686-elf-ld (cross-compiler, provided in cross/)
nasm  (>= 2.14)
GNU make
QEMU  (for emulation)
```

### Build

**Linux/WSL:**
```bash
git clone https://github.com/kazah-png/nyx-os.git
cd nyx-os
make -C kernel
```

**Windows (PowerShell):**
```powershell
.\build.ps1
```
*(Requires WSL with cross-compiler at `cross/bin/`)*

### Run in QEMU

**Quick serial test:**
```bash
qemu-system-i386 -kernel kernel/nyx-kernel.bin -m 256M -no-reboot -serial stdio
```

**With GUI (desktop):**
```bash
qemu-system-i386 -kernel kernel/nyx-kernel.bin -m 256M -no-reboot
```

**With networking:**
```bash
qemu-system-i386 -kernel kernel/nyx-kernel.bin -m 256M -nic user,model=rtl8139
```

**With sound + disk + network:**
```bash
qemu-system-i386 -kernel kernel/nyx-kernel.bin -m 256M -hda ext2-test.img -nic user,model=rtl8139 -soundhw sb16
```

**Windows (PowerShell):**
```powershell
.\run.ps1                  # GUI mode (default)
.\run.ps1 -Mode serial     # Serial debug output
.\run.ps1 -Mode net        # With RTL8139 networking
.\run.ps1 -Mode net -Sound # Networking + SB16 sound
```

---

## Status

See the full **[NyxOS Status Report](https://github.com/kazah-png/nyx-os/issues/1)** for a detailed feature checklist.

### What works
- ✅ Full boot sequence to GUI desktop (or text shell fallback)
- ✅ 40+ shell commands with Tab completion, env vars, pipes, history
- ✅ Ramdisk VFS + EXT2 read/write (auto-mount at /mnt)
- ✅ Real networking (RTL8139 + ARP/IP/UDP/ICMP/DHCP/TCP)
- ✅ Window compositor (32 windows, workspaces, taskbar, Start menu)
- ✅ Terminal emulator window with scrollback and command execution
- ✅ File Manager window with VFS directory browsing
- ✅ Interrupt-driven timer, keyboard, mouse
- ✅ Sound Blaster 16 DSP detection, DMA programming, mixer, PCM playback
- ✅ PC speaker tones and melodies
- ✅ DOOM game (VGA mode 13h, doomgeneric port)
- ✅ VBE framebuffer (1024x768x32)
- ✅ Bitmap font rendering
- ✅ ELF32 userspace loader with initramfs (auto-boot init.elf)
- ✅ 9 syscalls via int 0x80 (exit, write, print, open, read, close, getpid, sbrk, exec)
- ✅ Minimal C library for userspace (printf, malloc, free, snprintf, string ops)
- ✅ Real-time clock (RTC) driver with wall-clock time display
- ✅ Desktop polish (wallpaper, right-click context menu, Settings window, File Manager toolbar)

### What's being built
- 🔄 DOOM sound: wire DOOM sound module to SB16 output
- 🔄 Scrollbar in File Manager window
- 🔄 Drag-reorder desktop icons
- 🔄 Right-click context menu in File Manager (rename, copy, paste)

---

## Community

Join the **NyxOS Discord** to follow development, ask questions, or contribute:

[![Discord](https://img.shields.io/badge/Discord-NyxOS-5865F2?style=for-the-badge&logo=discord&logoColor=white)](https://dsc.gg/nyxos)

- **Server:** [dsc.gg/nyxos](https://dsc.gg/nyxos)
- **Topics:** Kernel development, OS design, networking, low-level programming
- **Channel:** `#nyxos-dev` for build issues and feature discussions

---

## License

This project is free software: you can redistribute it and/or modify it under the terms of the **GNU General Public License** as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

The kernel links against doomgeneric (Chocolate Doom-derived), which is GPL-2.0+. All original kernel code is also distributed under GPL-2.0+.

See the [LICENSE](LICENSE) file for the full license text.

---

<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=0:2d2d2d,50:1a1a1a,100:0a0a0a&height=80&section=footer" />
</div>
