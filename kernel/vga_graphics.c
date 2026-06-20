// ============================================================
// vga_graphics.c - Soporte para modo gráfico VGA 13h
// ============================================================
#include "kernel.h"

// Copia un buffer de 320x200 píxeles a la memoria de video VGA (0xA0000)
void vga_copy_buffer(uint8_t* buffer) {
    uint8_t* vga = (uint8_t*)0xA0000;
    // Usamos memcpy_asm (definida en boot.asm)
    memcpy_asm(vga, buffer, 320 * 200);
}

// Cambia a modo gráfico 13h (320x200, 256 colores)
void vga_set_mode_13h(void) {
    // Guardar registro de modo actual (no lo necesitamos)
    // Secuencia de inicialización para modo 13h (basada en VGA)
    outb(0x3C2, 0xE3);   // Misc Output Register
    outb(0x3D4, 0x00);
    outb(0x3D5, 0x5F);   // Horizontal Total (5Fh)
    outb(0x3D4, 0x01);
    outb(0x3D5, 0x4F);   // Horizontal Display End (4Fh)
    outb(0x3D4, 0x02);
    outb(0x3D5, 0x50);   // Start Horizontal Blank (50h)
    outb(0x3D4, 0x03);
    outb(0x3D5, 0x82);   // End Horizontal Blank (82h)
    outb(0x3D4, 0x04);
    outb(0x3D5, 0x54);   // Start Horizontal Sync (54h)
    outb(0x3D4, 0x05);
    outb(0x3D5, 0x80);   // End Horizontal Sync (80h)
    outb(0x3D4, 0x06);
    outb(0x3D5, 0x0B);   // Vertical Total (0Bh)
    outb(0x3D4, 0x07);
    outb(0x3D5, 0x3E);   // Overflow Register (3Eh)
    outb(0x3D4, 0x08);
    outb(0x3D5, 0x40);   // Preset Row Scan (40h)
    outb(0x3D4, 0x09);
    outb(0x3D5, 0x00);   // Maximum Scan Line (00h)
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x00);   // Cursor Start (00h)
    outb(0x3D4, 0x0B);
    outb(0x3D5, 0x00);   // Cursor End (00h)
    outb(0x3D4, 0x0C);
    outb(0x3D5, 0x00);   // Start Address High (00h)
    outb(0x3D4, 0x0D);
    outb(0x3D5, 0x00);   // Start Address Low (00h)
    outb(0x3D4, 0x0E);
    outb(0x3D5, 0x00);   // Cursor Location High (00h)
    outb(0x3D4, 0x0F);
    outb(0x3D5, 0x00);   // Cursor Location Low (00h)
    outb(0x3D4, 0x10);
    outb(0x3D5, 0x00);   // Vertical Retrace Start (00h)
    outb(0x3D4, 0x11);
    outb(0x3D5, 0x00);   // Vertical Retrace End (00h)
    outb(0x3D4, 0x12);
    outb(0x3D5, 0x00);   // Vertical Display End (00h)
    outb(0x3D4, 0x13);
    outb(0x3D5, 0x00);   // Offset (00h)
    outb(0x3D4, 0x14);
    outb(0x3D5, 0x00);   // Underline Location (00h)
    outb(0x3D4, 0x15);
    outb(0x3D5, 0x00);   // Start Vertical Blank (00h)
    outb(0x3D4, 0x16);
    outb(0x3D5, 0x00);   // End Vertical Blank (00h)
    outb(0x3D4, 0x17);
    outb(0x3D5, 0xE3);   // Mode Control (E3h)
    outb(0x3D4, 0x18);
    outb(0x3D5, 0xFF);   // Line Compare (FFh)

    // Configurar secuenciador
    outb(0x3C4, 0x01);
    outb(0x3C5, 0x01);   // Clock Mode (01h)
    outb(0x3C4, 0x02);
    outb(0x3C5, 0x0F);   // Map Mask (0Fh)
    outb(0x3C4, 0x03);
    outb(0x3C5, 0x00);   // Character Map Select (00h)
    outb(0x3C4, 0x04);
    outb(0x3C5, 0x0E);   // Memory Mode (0Eh)

    // Configurar controlador de gráficos
    outb(0x3CE, 0x05);
    outb(0x3CF, 0x40);   // Mode Register (40h)
    outb(0x3CE, 0x06);
    outb(0x3CF, 0x05);   // Miscellaneous Register (05h)

    // Paleta de colores por defecto (0-255)
    // No la configuramos, asumimos la paleta estándar.
}