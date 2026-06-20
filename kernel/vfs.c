// vfs.c - Stubs para operaciones de archivo
#include "kernel.h"

static int fd_counter = 0;

void init_vfs(void) {
    // Nada
}

int vfs_open(const char* path, int flags, mode_t mode) {
    (void)path; (void)flags; (void)mode;
    return ++fd_counter; // Simula un descriptor
}

int vfs_read(int fd, void* buf, size_t count) {
    (void)fd; (void)buf; (void)count;
    return 0;
}

int vfs_write(int fd, const void* buf, size_t count) {
    (void)fd; (void)buf; (void)count;
    return count; // Simula escritura exitosa
}

int vfs_close(int fd) {
    (void)fd;
    return 0;
}

int vfs_mkdir(const char* path, mode_t mode) {
    (void)path; (void)mode; return 0;
}

int vfs_unlink(const char* path) {
    (void)path; return 0;
}

dirent_t* vfs_readdir(int fd) {
    (void)fd; return NULL;
}

void hide_file(const char* path) {
    (void)path;
}

void vfs_rename(const char* old, const char* new) {
    (void)old; (void)new;
}