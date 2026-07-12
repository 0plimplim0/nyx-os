#include "kernel.h"
#include "elf.h"

/* Shared libc (v5.8.28). One copy of libc's code is loaded at boot (from the
 * initramfs file /libc.so, prelinked at SHARED_LIBC_BASE) and its read-only
 * segments are mapped — SHARED, same physical frames — into every user process.
 * libc's one writable page (.bss = the malloc freelist head) gets a private
 * per-process copy. User programs are linked with `ld --just-symbols=libc.so`, so
 * their calls to printf/malloc/... resolve to libc's fixed addresses without
 * bundling its ~14 KB of code. See kernel.h and the makefile. */

#define MAX_SEGS       8
#define MAX_SEG_PAGES  32

typedef struct {
    uint64_t vaddr;                  /* page-aligned base VA                    */
    uint32_t npages;
    int      writable;               /* 1 = private per-process copy (.bss/.data)*/
    int      exec;                   /* 1 = executable (.text)                   */
    void*    frames[MAX_SEG_PAGES];  /* master frames, content loaded at boot    */
} libc_seg_t;

static libc_seg_t segs[MAX_SEGS];
static int nsegs = 0;
static int libc_ready = 0;

int shared_libc_is_ready(void) { return libc_ready; }

/* Parse /libc.so and load each PT_LOAD segment into freshly-allocated physical
 * frames (the master image). RO segments are shared into processes as-is; the
 * writable segment is copied per process. Called once at boot, before any user
 * process is created. */
void shared_libc_load(void) {
    int fd = vfs_open("/libc.so", 0, 0);
    if (fd < 0) { serial_puts("[LIBC] /libc.so not found\n"); return; }
    uint32_t size = vfs_fsize(fd);
    uint8_t* data = vfs_fdata(fd);
    if (!data || size == 0 || !elf_validate(data, size)) {
        vfs_close(fd); serial_puts("[LIBC] /libc.so invalid\n"); return;
    }

    elf64_hdr_t* hdr = (elf64_hdr_t*)data;
    elf64_phdr_t* ph = (elf64_phdr_t*)(data + hdr->e_phoff);

    for (uint32_t i = 0; i < hdr->e_phnum && nsegs < MAX_SEGS; i++) {
        if (ph[i].p_type != PT_LOAD) continue;
        uint64_t vaddr  = ph[i].p_vaddr;
        uint64_t memsz  = ph[i].p_memsz;
        uint64_t filesz = ph[i].p_filesz;
        uint64_t offset = ph[i].p_offset;
        uint64_t start  = vaddr & ~0xFFFULL;
        uint64_t end    = (vaddr + memsz + 0xFFF) & ~0xFFFULL;
        uint32_t np     = (uint32_t)((end - start) / 4096);
        if (np > MAX_SEG_PAGES) np = MAX_SEG_PAGES;

        libc_seg_t* s = &segs[nsegs];
        s->vaddr    = start;
        s->npages   = np;
        s->writable = (ph[i].p_flags & PF_W) ? 1 : 0;
        s->exec     = (ph[i].p_flags & PF_X) ? 1 : 0;

        for (uint32_t p = 0; p < np; p++) {
            void* frame = alloc_page();
            if (!frame) { vfs_close(fd); return; }
            memset_asm(frame, 0, 4096);
            uint64_t page = start + p * 4096;
            uint64_t cs = (page > vaddr) ? page : vaddr;                 /* copy window */
            uint64_t ce = (page + 4096 < vaddr + filesz) ? page + 4096 : vaddr + filesz;
            if (cs < ce)
                memcpy_asm((uint8_t*)frame + (cs - page), data + offset + (cs - vaddr), ce - cs);
            s->frames[p] = frame;
        }
        nsegs++;
    }
    vfs_close(fd);
    libc_ready = 1;
    serial_puts("[LIBC] shared libc mapped at 0x30000000\n");
}

/* Map the shared libc into a process's address space (called from elf_load_image
 * for every spawn/execve; fork inherits it via clone_page_directory_cow). RO
 * segments point at the shared master frames (page_incref keeps the refcount
 * correct across process teardown); the writable segment is a private copy. */
void shared_libc_map(uint64_t* pml4) {
    if (!libc_ready || !pml4) return;
    for (int i = 0; i < nsegs; i++) {
        libc_seg_t* s = &segs[i];
        for (uint32_t p = 0; p < s->npages; p++) {
            uint64_t va = s->vaddr + (uint64_t)p * 4096;
            if (s->writable) {
                void* fresh = alloc_page();
                if (!fresh) return;
                memcpy_asm(fresh, s->frames[p], 4096);           /* private .bss/.data */
                map_page_dir(pml4, fresh, (void*)va, 0x7 | PAGE_NX);
            } else {
                map_page_ro(pml4, s->frames[p], (void*)va, s->exec);
                page_incref(s->frames[p]);                        /* shared frame ref */
            }
        }
    }
}
