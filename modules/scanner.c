// ============================================================
// modules/scanner.c - Escáner de red (simulado)
// ============================================================
#include "modules.h"

static uint32_t pseudo_rand = 12345;

static uint32_t rand(void) {
    pseudo_rand = pseudo_rand * 1103515245 + 12345;
    return (pseudo_rand >> 16) & 0x7FFF;
}

void stealth_scan(uint32_t target_ip, uint16_t start_port, uint16_t end_port) {
    printf("[Scanner] Escaneando %u.%u.%u.%u puertos %d-%d\n",
           (target_ip>>24)&0xFF, (target_ip>>16)&0xFF,
           (target_ip>>8)&0xFF, target_ip&0xFF,
           start_port, end_port);
    // Simulación: mostrar puertos abiertos
    for (uint16_t port = start_port; port <= end_port; port++) {
        if (port % 1000 == 0) {
            printf("[+] Puerto %d abierto (simulado)\n", port);
        }
    }
}

void* module_scanner_init(void) {
    init_raw_socket();
    return NULL;
}