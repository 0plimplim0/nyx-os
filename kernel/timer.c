// ============================================================
// timer.c - Temporizador del sistema NyxOS (PIT a 1000 Hz por sondeo)
// ============================================================
#include "kernel.h"

static uint32_t timer_ticks = 0;
static uint32_t timer_frequency = 1000;
static uint32_t pit_divisor = 0;
static uint16_t last_pit_count = 0;

void init_timer(uint32_t frequency) {
    timer_frequency = frequency;
    timer_ticks = 0;
    pit_divisor = 1193180 / frequency;

    // Configurar PIT en modo 3 (square wave)
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(pit_divisor & 0xFF));
    outb(0x40, (uint8_t)((pit_divisor >> 8) & 0xFF));

    // Leer valor inicial del contador
    outb(0x43, 0x00);   // latch counter 0
    last_pit_count = inb(0x40);
    last_pit_count |= (inb(0x40) << 8);

    printf("[TIMER] %d Hz (polling)\n", frequency);
}

void update_timer(void) {
    uint16_t current;
    uint32_t diff;

    // Latch y leer el contador 0
    outb(0x43, 0x00);
    current = inb(0x40);
    current |= (inb(0x40) << 8);

    // Calcular decrementos desde la última lectura
    if (current <= last_pit_count) {
        diff = last_pit_count - current;
    } else {
        // wrap-around: el contador se recargó
        diff = (last_pit_count + pit_divisor) - current;
    }
    last_pit_count = current;

    // Acumular y convertir a ticks del sistema
    static uint32_t accumulated = 0;
    accumulated += diff;
    while (accumulated >= pit_divisor) {
        accumulated -= pit_divisor;
        timer_ticks++;
        tick_count = timer_ticks;
    }
}

uint32_t get_ticks(void) {
    return timer_ticks;
}

void sleep(uint32_t milliseconds) {
    uint32_t start = timer_ticks;
    uint32_t ticks_to_wait = (milliseconds * timer_frequency) / 1000;
    while ((timer_ticks - start) < ticks_to_wait) {
        update_timer();
        __asm__ volatile("hlt");
    }
}