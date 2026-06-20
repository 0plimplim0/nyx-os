#include "kernel.h"

#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))

static const uint32_t K256[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
    0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
    0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
    0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
    0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
    0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
    0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
    0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
    0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

static uint32_t ROTRIGHT(uint32_t a, uint32_t b) {
    return (a >> b) | (a << (32 - b));
}

static void sha256_transform(uint32_t state[8], const uint8_t block[64]) {
    uint32_t w[64];
    for (int i = 0; i < 16; i++) {
        w[i] = ((uint32_t)block[i*4] << 24) | ((uint32_t)block[i*4+1] << 16) |
               ((uint32_t)block[i*4+2] << 8) | (uint32_t)block[i*4+3];
    }
    for (int i = 16; i < 64; i++) {
        w[i] = SIG1(w[i-2]) + w[i-7] + SIG0(w[i-15]) + w[i-16];
    }
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
    uint32_t e = state[4], f = state[5], g = state[6], h = state[7];
    for (int i = 0; i < 64; i++) {
        uint32_t t1 = h + EP1(e) + CH(e,f,g) + K256[i] + w[i];
        uint32_t t2 = EP0(a) + MAJ(a,b,c);
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }
    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    state[4] += e; state[5] += f; state[6] += g; state[7] += h;
}

void sha256(const uint8_t* data, size_t len, uint8_t* hash) {
    uint32_t state[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    uint64_t bitlen = len * 8;
    size_t offset = 0;
    uint8_t block[64];

    while (len >= 64) {
        sha256_transform(state, data + offset);
        offset += 64;
        len -= 64;
    }
    size_t remaining = len;
    for (size_t i = 0; i < remaining; i++) block[i] = data[offset + i];
    block[remaining] = 0x80;
    if (remaining >= 56) {
        for (size_t i = remaining + 1; i < 64; i++) block[i] = 0;
        sha256_transform(state, block);
        for (int i = 0; i < 56; i++) block[i] = 0;
    } else {
        for (size_t i = remaining + 1; i < 56; i++) block[i] = 0;
    }
    for (int i = 0; i < 8; i++) block[56 + i] = (bitlen >> (56 - i*8)) & 0xFF;
    sha256_transform(state, block);

    for (int i = 0; i < 32; i++) {
        hash[i] = (state[i/4] >> (24 - (i%4)*8)) & 0xFF;
    }
}

// MD5 implementation
#define F(x,y,z) (((x) & (y)) | (~(x) & (z)))
#define G(x,y,z) (((x) & (z)) | ((y) & ~(z)))
#define H(x,y,z) ((x) ^ (y) ^ (z))
#define I(x,y,z) ((y) ^ ((x) | ~(z)))

#define MD5_FF(a,b,c,d,x,s,ac) { a += F(b,c,d) + x + ac; a = ROTLEFT(a,s); a += b; }
#define MD5_GG(a,b,c,d,x,s,ac) { a += G(b,c,d) + x + ac; a = ROTLEFT(a,s); a += b; }
#define MD5_HH(a,b,c,d,x,s,ac) { a += H(b,c,d) + x + ac; a = ROTLEFT(a,s); a += b; }
#define MD5_II(a,b,c,d,x,s,ac) { a += I(b,c,d) + x + ac; a = ROTLEFT(a,s); a += b; }

static const uint32_t MD5_K[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

static const uint32_t MD5_S[64] = {
    7,12,17,22, 7,12,17,22, 7,12,17,22, 7,12,17,22,
    5,9,14,20, 5,9,14,20, 5,9,14,20, 5,9,14,20,
    4,11,16,23, 4,11,16,23, 4,11,16,23, 4,11,16,23,
    6,10,15,21, 6,10,15,21, 6,10,15,21, 6,10,15,21
};

static void md5_transform(uint32_t state[4], const uint8_t block[64]) {
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
    uint32_t x[16];
    for (int i = 0; i < 16; i++) {
        x[i] = (uint32_t)block[i*4] | ((uint32_t)block[i*4+1] << 8) |
               ((uint32_t)block[i*4+2] << 16) | ((uint32_t)block[i*4+3] << 24);
    }
    for (int i = 0; i < 64; i++) {
        int g, s = MD5_S[i];
        uint32_t k = MD5_K[i];
        if (i < 16) {
            g = i;
            MD5_FF(a,b,c,d,x[g],s,k);
        } else if (i < 32) {
            g = (5*i + 1) % 16;
            MD5_GG(a,b,c,d,x[g],s,k);
        } else if (i < 48) {
            g = (3*i + 5) % 16;
            MD5_HH(a,b,c,d,x[g],s,k);
        } else {
            g = (7*i) % 16;
            MD5_II(a,b,c,d,x[g],s,k);
        }
        uint32_t tmp = d; d = c; c = b; b = a; a = tmp;
    }
    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
}

void md5(const uint8_t* data, size_t len, uint8_t* hash) {
    uint32_t state[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
    uint64_t bitlen = len * 8;
    size_t offset = 0;
    uint8_t block[64];

    while (len >= 64) {
        md5_transform(state, data + offset);
        offset += 64;
        len -= 64;
    }
    size_t remaining = len;
    for (size_t i = 0; i < remaining; i++) block[i] = data[offset + i];
    block[remaining] = 0x80;
    if (remaining >= 56) {
        for (size_t i = remaining + 1; i < 64; i++) block[i] = 0;
        md5_transform(state, block);
        for (int i = 0; i < 56; i++) block[i] = 0;
    } else {
        for (size_t i = remaining + 1; i < 56; i++) block[i] = 0;
    }
    for (int i = 0; i < 8; i++) block[56 + i] = (bitlen >> (i * 8)) & 0xFF;
    md5_transform(state, block);

    for (int i = 0; i < 16; i++) {
        hash[i] = (state[i/4] >> ((i%4)*8)) & 0xFF;
    }
}

void aes_encrypt(const uint8_t* key, const uint8_t* input, uint8_t* output) {
    (void)key; (void)input; (void)output;
}

void aes_decrypt(const uint8_t* key, const uint8_t* input, uint8_t* output) {
    (void)key; (void)input; (void)output;
}

void init_crypto(void) {
}
