# Version History

| Version | Description |
|---------|-------------|
| v1.0.0 | Base kernel, VGA text mode, GDT/IDT/ISR/IRQ, PIT timer, PS/2 keyboard, serial, bitmap physical allocator, identity paging, shell with basic commands |
| v1.1.0 | Ramdisk VFS + shell commands (ls/cd/cat/touch/mkdir/rm/cp/mv/echo/clear/help) |
| v1.2.0 | Real networking (RTL8139, ARP, IP, UDP, ICMP/ping) |
| v2.0.0 | Clean slate: removed all hacking/offensive code |
| v2.0.1 | DOOM game integration (VGA mode 13h, doomgeneric, shell command) |
| v2.1.0 | Preemptive multitasking, interrupt-driven I/O, pipe support, diff command |
| v2.1.1 | Full TCP stack, RTL8139 fixes, GUI/window compositor, PC speaker, bitmap font |
| v2.2.0 | GUI auto-boot: NyxOS Desktop at startup instead of text shell |
| v2.3.0 | Real EXT2 read support: VFS mount layer, auto-mount at /mnt, ls/cd/cat on ext2 |
| v2.4.0 | Sound Blaster 16 DMA/IRQ audio driver (sb16play command) |
| v3.0.0 | ELF userspace loader, initramfs, per-process paging, ring 3 execution, int 0x80 syscalls |
| v3.1.0 | RTC driver, more syscalls, libc, initramfs auto-boot, desktop polish |
| v4.0.0 | Full x86_64 native (long mode, 4-level paging, syscall/sysret, ELF64, ring 3) |
| v4.1.0 | Higher-half kernel mapping, user/kernel page-table isolation, CR3 switching |
| v4.2.0 | NX bit + SMEP, Local APIC + I/O APIC, CPUID, IST for double fault/NMI |
| v5.0.0 | Full GUI suite: Text Editor, Image Viewer, Sound Test. No crashes, 0 warnings |
| v5.1.0 | Stability: TSS alignment fix, GDT limit, APIC/PIC masking, VGA font fix |
| v5.3.0 | Login system: boot animation, EXT2 credential storage, framebuffer login screen |
| v5.4.0 | Security hardening: syscall validation, opaque fds, build warning-free |
| v5.5.0 | Ring-3 runs: ELF loader fix, syscall entry rewrite, copy_from_user, iretq return |
| v5.6.0 | Process reaping fix, heap bug fix (slab fallback), user file I/O |
| v5.7.0 | Per-fd offsets, streaming file I/O |
| v5.7.1 | Fd-based I/O routes to EXT2 mounts (persistent files) |
| v5.7.2 | Fd-based readdir on EXT2 mounts (file-manager can browse disk) |
| v5.7.3 | DHCP networking (7 bugs fixed: IP checksum, ARP, RTL8139 RX, DHCP cookie, poll delay) |
| v5.7.4 | Full network stack: DNS, TCP, HTTP (byte-order fixes, ARP, routing) |
| v5.7.5 | GUI stability: CR3 switch in irq_common fixed, boot stack enlarged |
| v5.7.6 | Preemptive scheduler + timer (PIT via I/O APIC pin 2), mtdemo |
| v5.7.7 | Preemptive ring-3 processes, preempt_disable guards, kernel stack alias fix |
| v5.7.8 | wait()/job control: foreground (exec+kwait) and background (spawn) |
| v5.7.9 | Legacy blocking-exec path (setjmp/longjmp) removed |
| v5.7.10 | Blocked sleep() + safe kill via zombie/wake_waiters |
| v5.7.11 | Shell job control: jobs/wait commands |
| v5.7.12 | Priority scheduling (weighted round-robin) |
| v5.7.13 | VFS re-entrancy guard (preempt_disable) |
| v5.7.14 | Per-process file descriptors |
| v5.7.15 | Idle desktop yields CPU + nice/renice |
| v5.7.16 | File Manager drag-and-drop into folders |
| v5.7.17 | Loopback + ICMP echo replies + real ping |
| v5.7.18 | TCP retransmission (RTO) + robust multi-segment receive |
| v5.7.19 | TCP passive open (listen/accept) + in-guest loopback TCP |
| v5.7.20 | NIC-side TCP listen: NyxOS serves HTTP to real clients |
| v5.7.21 | Per-shell CWD + full-path pwd/prompt |
| v5.7.22 | SMP: multi-core bringup + cpus command |
| v5.7.23 | Demand paging + copy-on-write (PTE_DEMAND, PTE_COW, cowtest) |
