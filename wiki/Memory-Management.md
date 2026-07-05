# Memory Management

NyxOS uses a layered memory strategy: bitmap physical allocator, 4-level paging, kernel heap, and slab caches.

## Physical allocator (`memory.c`)
Bitmap tracking each 4 KB page, up to 512 MB max RAM. `alloc_page()` / `free_page()`.

## 4-level paging (`paging.c`)
PML4 → PDPT → PD → PT → 4 KB page. Higher-half kernel at PML4[511]. Identity-mapped first 64 MB.

- **Demand paging:** PTE bit 9 (`PTE_DEMAND`) — allocate zeroed page on first touch via #PF handler
- **Copy-on-write:** PTE bit 10 (`PTE_COW`) — private copy on write fault via #PF handler
- **NX bit:** Non-executable user stack/data pages
- **CR0.WP:** Supervisor writes to read-only pages fault (required for COW)

## Kernel heap (`heap.c`)
16 MB heap with free-list, header/footer metadata, coalescing. `kmalloc`/`kfree` with slab fallback.

## Slab allocator (`slab.c`)
Fixed-size caches: 8, 16, 32, 64, 128, 256, 512, 1024 bytes.
