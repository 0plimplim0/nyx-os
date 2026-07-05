# Userspace

Ring-3 ELF64 loading with full address-space isolation.

## ELF loader (`elf.c`)
Validates ELF64 magic, parses PT_LOAD segments, maps code/data at p_vaddr, allocates stack at 0xD0000000.

## Process isolation
- Private PML4 per process (no identity mapping in user tables)
- NX bit on user stack and data pages
- SMEP (optional): kernel can't execute user pages
- Kernel entry at PML4[511] cloned from master kernel PML4

## libc (`user/libc.c`)
Minimal C library: `printf`, `snprintf`, `malloc`, `free`, file I/O wrappers via syscalls.

## crt0 (`user/crt0.asm`)
Assembly entry that extracts argc/argv from stack, calls `main()`, then `exit()`.

## Available programs
| Binary | Description |
|--------|-------------|
| `hello.elf` | Minimal "Hello World" in assembly |
| `init.elf` | Syscall regression test (getpid, malloc, file I/O) |
| `spin.elf` | Spin loop for multitasking testing |
| `fdleak.elf` | Tests fd cleanup on process exit |
