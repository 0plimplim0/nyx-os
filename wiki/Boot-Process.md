# Boot Process

## Stage 1: GRUB
NyxOS uses the **Multiboot** protocol. The kernel starts with a Multiboot header in `boot.asm`. GRUB loads the kernel, sets up protected mode, fills the Multiboot info structure, and jumps to the kernel entry point.

## Stage 2: Real mode → Protected mode → Long mode (`boot.asm`)

1. Clears EFLAGS, sets up minimal GDT
2. Checks for Long Mode via CPUID
3. Enables PAE in CR4
4. Creates temporary PML4 with first 2 MB identity-mapped
5. Loads PML4 into CR3
6. Sets EFER.LME (Long Mode Enable)
7. Enables paging (CR0.PG)
8. Sets up 64-bit GDT (kernel ring 0 + user ring 3 segments)
9. Far jump to reload CS
10. Sets up segment registers and stack
11. Zeroes BSS
12. Calls `kernel_main()`

## Stage 3: `kernel_main()` initialisation

```
init_gdt() → init_idt() → init_isr() → init_irq() → init_memory() →
init_paging() → init_heap() → init_slab() → vbe_init() → fb_init() →
init_apic() → init_timer(1000) → init_keyboard() → init_process() →
ensure_idle_process() → init_syscalls() → init_vfs() → init_ext2() →
init_net() → tcp_init() → init_background_tasks() → mouse_init() →
speaker_init() → sb16_init() → sti → initramfs_load() →
initramfs_boot() → bootsplash_update() → auth_setup() →
login_screen() → compositor_init() + compositor_run()
```

## Stage 4: Boot animation
~5 second splash with NyxOS owl logo, 60 twinkling stars, spinning indicator, 23-step progress bar. Fades to black before login.

## Stage 5: Login
Framebuffer login with username/password fields. Default `nyx`/`nyx`. Three failures → reboot. Credentials in `/etc/passwd` on EXT2 (djb2 + salt).

## Stage 6: Desktop
Compositor starts with 8 app icons, taskbar, Start menu, 4 workspaces.
