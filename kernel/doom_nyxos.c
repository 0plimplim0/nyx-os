// ============================================================
// doom_nyxos.c - Capa de adaptación de DOOM para NyxOS
// ============================================================
#include "kernel.h"
#include "doom_src/doom.h"

uint8_t screenBuffer[320 * 200];

void DG_Init(void) {
    vga_set_mode_13h();
}

void DG_DrawFrame(void) {
    vga_copy_buffer(screenBuffer);
}

int DG_GetKey(void) {
    char c = getchar();
    switch (c) {
        case 'w': return 0x48;
        case 's': return 0x50;
        case 'a': return 0x4B;
        case 'd': return 0x4D;
        case ' ': return 0x39;
        case 'q': return 0x10;
        case 'e': return 0x12;
        default: return (int)c;
    }
}

void DG_SleepMs(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 10000; i++);
}

void DG_SetWindowTitle(const char* title) {
    (void)title;
}

void run_doom(void) {
    DG_Init();
    D_DoomMain();   // Función principal de DOOM
    init_screen();
    clear_screen();
}