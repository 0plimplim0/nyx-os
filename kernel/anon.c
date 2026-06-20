// anon.c
#include "kernel.h"

void init_anon(void) {
    // Nada
}

void tor_enable_system_wide(uint16_t socks, uint16_t ctrl) {
    (void)socks; (void)ctrl;
}

void mac_spoof_schedule(uint32_t interval) {
    (void)interval;
}

void hostname_randomize(void) { }
void disable_ipv6_leaks(void) { }
void disable_webrtc_leaks(void) { }
void disable_dns_leaks(void) { }
void dnscrypt_enable(const char* dns1, const char* dns2) {
    (void)dns1; (void)dns2;
}