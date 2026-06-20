// ============================================================
// net.c - Pila de red de NyxOS
// ============================================================
#include "kernel.h"

// Referencia a la variable global definida en kernel.c
extern net_iface_t net_interfaces[8];

#define MAX_SOCKETS 64

typedef struct {
    int fd;
    int domain;
    int type;
    int protocol;
    uint32_t local_ip;
    uint16_t local_port;
    uint32_t remote_ip;
    uint16_t remote_port;
    int state;  // 0=closed, 1=listening, 2=connected
    void* rx_buffer;
    void* tx_buffer;
} socket_t;

static socket_t sockets[MAX_SOCKETS];
static int socket_count = 0;

void init_net(void) {
    memset_asm(sockets, 0, sizeof(sockets));
    memset_asm(net_interfaces, 0, sizeof(net_interfaces));
    printf("[NET] Network stack initialized\n");
}

int net_create_socket(int domain, int type, int protocol) {
    if (socket_count >= MAX_SOCKETS) return -1;
    int idx = socket_count++;
    sockets[idx].fd = idx;
    sockets[idx].domain = domain;
    sockets[idx].type = type;
    sockets[idx].protocol = protocol;
    sockets[idx].state = 0;
    return idx;
}

int net_bind(int sock, uint32_t ip, uint16_t port) {
    if (sock < 0 || sock >= MAX_SOCKETS) return -1;
    sockets[sock].local_ip = ip;
    sockets[sock].local_port = port;
    return 0;
}

int net_listen(int sock, int backlog) {
    (void)backlog;
    if (sock < 0 || sock >= MAX_SOCKETS) return -1;
    sockets[sock].state = 1;
    return 0;
}

int net_accept(int sock) {
    if (sock < 0 || sock >= MAX_SOCKETS) return -1;
    return net_create_socket(2, 1, 0);
}

int net_connect(int sock, uint32_t ip, uint16_t port) {
    if (sock < 0 || sock >= MAX_SOCKETS) return -1;
    sockets[sock].remote_ip = ip;
    sockets[sock].remote_port = port;
    sockets[sock].state = 2;
    return 0;
}

int net_send(int sock, const void* buf, size_t len) {
    (void)buf;
    if (sock < 0 || sock >= MAX_SOCKETS) return -1;
    return len;
}

int net_recv(int sock, void* buf, size_t len) {
    (void)buf;
    if (sock < 0 || sock >= MAX_SOCKETS) return -1;
    return len;
}

int net_close(int sock) {
    if (sock < 0 || sock >= MAX_SOCKETS) return -1;
    sockets[sock].state = 0;
    return 0;
}