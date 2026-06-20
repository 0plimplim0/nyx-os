// ============================================================
// heap.c - Allocador de montículo (first-fit)
// ============================================================
#include "kernel.h"

typedef struct heap_block {
    size_t size;
    uint8_t used;
    struct heap_block* next;
} heap_block_t;

static uint8_t heap[KERNEL_HEAP_SIZE];
static heap_block_t* free_list = (heap_block_t*)heap;

void init_heap(void) {
    free_list->size = KERNEL_HEAP_SIZE - sizeof(heap_block_t);
    free_list->used = 0;
    free_list->next = NULL;
}

void* kmalloc(size_t size) {
    heap_block_t* curr = free_list;
    while (curr) {
        if (!curr->used && curr->size >= size) {
            // Dividir bloque si sobra
            if (curr->size > size + sizeof(heap_block_t) + 16) {
                heap_block_t* new_block = (heap_block_t*)((uint8_t*)curr + sizeof(heap_block_t) + size);
                new_block->size = curr->size - size - sizeof(heap_block_t);
                new_block->used = 0;
                new_block->next = curr->next;
                curr->next = new_block;
                curr->size = size;
            }
            curr->used = 1;
            return (void*)((uint8_t*)curr + sizeof(heap_block_t));
        }
        curr = curr->next;
    }
    return NULL;
}

void kfree(void* ptr) {
    if (!ptr) return;
    heap_block_t* block = (heap_block_t*)((uint8_t*)ptr - sizeof(heap_block_t));
    block->used = 0;
    // Coalescer con siguiente si está libre
    if (block->next && !block->next->used) {
        block->size += sizeof(heap_block_t) + block->next->size;
        block->next = block->next->next;
    }
    // Coalescer con anterior (no implementado por simplicidad)
}