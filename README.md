<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=0:0a0a0a,50:1a1a1a,100:2d2d2d&height=140&section=header&text=NyxOS&fontSize=52&fontColor=00ff9d&animation=fadeIn&fontAlignY=55" />
</div>

<div align="center">
  <strong>Custom 32-bit x86 kernel · C and Assembly · Offensive security research platform</strong>
  <br/><br/>
  <a href="https://github.com/kazah-png/nyx-os">
    <img src="https://img.shields.io/badge/repository-view%20all-0d1117?style=flat&logo=github&logoColor=white" />
  </a>
  <img src="https://img.shields.io/badge/version-1.0.0-00ff9d?style=flat" />
  <img src="https://img.shields.io/badge/arch-i686-00ff9d?style=flat" />
</div>

---

## About

**NyxOS** is a from-scratch 32-bit x86 kernel built as a research platform for low-level systems programming and offensive security. It boots via Multiboot (GRUB-compatible), runs in protected mode with paging, and provides a modular framework for loading kernel-level components.

The project implements core kernel primitives, a custom TCP/IP stack, an EXT2 filesystem driver, and a collection of offensive security modules — all written in C and x86 Assembly with no external libraries.

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
![EXT2](https://img.shields.io/badge/EXT2-8B4513?style=flat)

**Network stack**

![Ethernet](https://img.shields.io/badge/Ethernet-2496ED?style=flat)
![ARP](https://img.shields.io/badge/ARP-00ADD8?style=flat)
![IP](https://img.shields.io/badge/IP-00599C?style=flat)
![UDP](https://img.shields.io/badge/UDP-512BD4?style=flat)
![TCP](https://img.shields.io/badge/TCP-3178C6?style=flat)

**Crypto**

![MD5](https://img.shields.io/badge/MD5-3776AB?style=flat)
![SHA256](https://img.shields.io/badge/SHA256-CE422B?style=flat)
![AES](https://img.shields.io/badge/AES-00ADD8?style=flat)

---

## Features

### Boot & initialization
- Multiboot-compliant (GRUB-ready)
- Protected mode 32-bit with GDT setup (code/data segments)
- Paging with identity mapping + higher-half kernel layout
- Full IDT with exception handlers (0-31) and IRQ remapping (32-47)

### Memory management
- Physical memory manager with page-stack allocation
- Kernel heap (`kmalloc`/`kfree`) with first-fit + block splitting + coalescing
- Page table manipulation

### Process management
- Static process table (up to 512 processes)
- PID/PPID tracking, process states, stealth levels
- Process creation, destruction, hiding

### Shell & commands
Built-in command interpreter with 25+ commands:

`help` `version` `clear` `nyxfetch` `echo` `hexdump` `date` `uname` `scan` `hash` `exploit` `brute` `memscan` `shellcode` `keylog` `backdoor` `bdshell` `reboot` `ps` `mem` `modules` `crash` `layout`

Features: Spanish and US keyboard layouts, AltGr support, VGA text mode with color support, PIT timer (polling).

### Network stack (simulated)
Custom Ethernet, ARP, IP, UDP, TCP implementation. Socket API (`socket`, `bind`, `listen`, `accept`, `connect`, `send`, `recv`).

### Cryptographic subsystem
- SHA-256 (FIPS 180-4 compliant)
- MD5 (RFC 1321 compliant)
- AES stubs

### Offensive modules
Each module is a kernel-loadable component:

| Module | Description |
|--------|-------------|
| **Rootkit** | Hides processes and files matching keywords |
| **Backdoor** | Persistent reverse shell with password auth |
| **Keylogger** | Captures keystrokes to kernel buffer |
| **Injector** | Shellcode injection into user-space processes |
| **Scanner** | Port scanner and vulnerability detector |
| **Exploit loader** | Simulated CVE-based exploit engine |
| **Hydra brute** | Dictionary attack simulation |
| **C2 server** | Command-and-control endpoint |
| **Ransomware** | File encryption demonstration |
| **Cryptominer** | CPU-based miner (simulated) |
| **Reaver** | WPS PIN attack simulation |

---

## Project structure

```
nyx-os/
├── kernel/
│   ├── boot.asm          # Multiboot header, entry point
│   ├── kernel.c          # Main kernel, shell, command dispatcher
│   ├── kernel.h          # Core header (types, structs, inline funcs)
│   ├── gdt.c / gdt_flush.asm
│   ├── idt.c / idt_load.asm
│   ├── isr.c / isr_stubs.asm
│   ├── irq.c
│   ├── memory.c          # Physical memory manager
│   ├── heap.c            # Kernel heap allocator
│   ├── paging.c          # Page tables, virtual memory
│   ├── process.c         # Process management
│   ├── syscall.c         # System calls
│   ├── vfs.c / ext2.c   # Virtual file system / EXT2
│   ├── net.c / tcp.c / udp.c / ip.c / ethernet.c  # Network stack
│   ├── crypto.c          # SHA-256, MD5, AES
│   ├── anon.c            # Anonymity subsystem (Tor, MAC, DNS)
│   ├── timer.c           # PIT timer
│   ├── keyboard.c        # PS/2 driver (US/ES layouts)
│   ├── screen.c          # VGA text mode
│   ├── serial.c          # COM1 debug output
│   └── doom_nyxos.c      # DOOM integration
├── modules/
│   ├── modules.h
│   ├── rootkit.c / backdoor.c / keylogger.c / injector.c
│   ├── scanner.c / reaver.c / exploit_loader.c / hydra_brute.c
│   ├── c2_server.c / ransomware.c / cryptominer.c
│   └── Makefile
├── tools/
│   ├── build.sh
│   ├── qemu_launch.sh
│   └── qemu_launch.ps1
├── testing/
│   ├── dockerfile / docker-compose.yml
│   ├── test_suite/
│   └── vagrant/
├── Makefile              # Top-level build
└── README.md
```

---

## Build & run

### Prerequisites

- `i686-elf-gcc` / `i686-elf-ld` (cross-compiler)
- `nasm` (>= 2.14)
- GNU `make`
- QEMU (for emulation)

### Build

```bash
# Build the cross-compiler toolchain (one time only)
make toolchain

# Build the kernel and all modules
make

# Or build individual components
make kernel
make modules
make iso
```

### Run in QEMU

```bash
qemu-system-i386 -kernel kernel/nyx-kernel.bin -m 256M
```

Or use the helper script:
```bash
./tools/qemu_launch.sh
```

### Testing

```bash
make test
```

---

## Shell commands

| Command | Description |
|---------|-------------|
| `help` | Show available commands |
| `version` | Display kernel version |
| `clear` | Clear the screen |
| `nyxfetch` | System info with ASCII logo |
| `echo <text>` | Print text |
| `hexdump <addr> [bytes]` | Dump memory contents |
| `date` | Show current date/time |
| `uname` | Show kernel information |
| `scan <ip>` | Port scan simulation |
| `hash <md5|sha256> <text>` | Generate hash |
| `exploit <cve>` | Run simulated exploit |
| `brute <user>` | Dictionary attack simulation |
| `memscan [start] [end]` | Scan memory for strings |
| `shellcode <hex>` | Execute shellcode (dangerous) |
| `keylog` | Dump captured keystrokes |
| `backdoor <on|off|status>` | Control backdoor |
| `reboot` | Reboot the system |
| `ps` | List processes |
| `mem` | Show memory usage |
| `modules` | List loaded modules |
| `crash` | Trigger kernel panic |
| `layout <us|es>` | Change keyboard layout |

---

## License

This project is provided for educational and research purposes only.

---

<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=0:2d2d2d,50:1a1a1a,100:0a0a0a&height=80&section=footer" />
</div>
