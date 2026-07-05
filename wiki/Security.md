# Security

NyxOS is a hobby OS with **no security guarantees**, but implements several hardening measures.

## Ring separation
| Ring | Privilege | Code | Memory |
|------|-----------|------|--------|
| 0 | Kernel | Scheduler, drivers, VFS | All physical memory + user memory via copy_from/to_user |
| 3 | User | ELF programs | Private user pages only |

## Memory protection
- **Page-table isolation:** Kernel in higher half (PML4[511]), user PML4s have no identity mapping
- **NX bit:** User stacks and data pages mapped non-executable
- **SMEP:** (Optional) Ring 0 cannot execute ring-3 pages
- **CR0.WP:** Supervisor writes to read-only pages fault (required for COW)

## Syscall hardening
- **Pointer validation:** `user_ptr_ok()` rejects pointers outside `[0x1000, 0x800000000000)`
- **Opaque fds:** Userspace gets small integers; kernel VFS handles never exposed
- **copy_from_user/copy_to_user:** Walks user page tables, bounces through identity map

## Authentication
Framebuffer login, `/etc/passwd` on EXT2 (XOR-obfuscated, djb2 + salt). Default `nyx`/`nyx`.

## Limitations
No IOMMU, no encryption, no Spectre mitigations, minimal TCP stack hardening.
