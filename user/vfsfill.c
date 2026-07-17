#include "libc.h"

/* vfsfill — stress the VFS per-directory child cap (v5.8.81 hardening test).
 *
 * Creates files in a fresh directory until open(O_CREAT) fails, proving the kernel
 * now REFUSES the insert (returns <0) once a directory reaches MAX_CHILDREN (or the
 * inode pool is exhausted) instead of writing past the fixed children[] array — an
 * unchecked `children[child_count++]` used to overflow into the adjacent inode.
 * Then it confirms the VFS is still intact (an existing, unrelated file still stats
 * correctly) and cleans up after itself so it can be re-run. */

#define DIR   "/tmp/fill"
#define LIMIT 400          /* safety bound so an unenforced cap can't loop forever */

/* write `v` as decimal at p (NUL-terminated); return the digit count */
static int put_uint(char* p, unsigned int v) {
    char tmp[12]; int n = 0;
    if (v == 0) tmp[n++] = '0';
    while (v) { tmp[n++] = (char)('0' + (v % 10)); v /= 10; }
    for (int i = 0; i < n; i++) p[i] = tmp[n - 1 - i];
    p[n] = '\0';
    return n;
}

/* build DIR "/f" <i> into buf */
static void make_path(char* buf, unsigned int i) {
    const char* d = DIR "/f";
    int j = 0;
    while (d[j]) { buf[j] = d[j]; j++; }
    put_uint(buf + j, i);
}

int main(void) {
    printf("vfsfill: filling %s until the VFS refuses (per-dir cap = MAX_CHILDREN)\n", DIR);
    mkdir("/tmp", 0);   /* ensure the parent exists (ok if it already does) */
    mkdir(DIR, 0);      /* the test directory (ok if it already exists) */

    char path[64];
    unsigned int created = 0;
    for (unsigned int i = 0; i < LIMIT; i++) {
        make_path(path, i);
        int fd = open(path, O_CREAT, 0);
        if (fd < 0) break;
        close(fd);
        created++;
    }

    if (created >= LIMIT)
        printf("vfsfill: hit the %d safety bound with NO refusal (cap not enforced?!)\n", LIMIT);
    else
        printf("vfsfill: created %u files, then open() returned <0 -> refused cleanly, no crash\n", created);

    /* Liveness: an existing, unrelated inode must be untouched (no OOB corruption). */
    struct stat st;
    if (stat("/sh.elf", &st) == 0)
        printf("vfsfill: VFS intact -- /sh.elf still stats (size=%u)\n", st.st_size);
    else
        printf("vfsfill: WARNING -- /sh.elf no longer stats (possible corruption!)\n");

    /* Clean up so the inode pool is returned and the test can be re-run. */
    for (unsigned int i = 0; i < created; i++) {
        make_path(path, i);
        unlink(path);
    }
    printf("vfsfill: cleaned up %u files. done.\n", created);
    return 0;
}
