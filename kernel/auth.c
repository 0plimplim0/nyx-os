#include "kernel.h"
#include "auth.h"
#include "ext2.h"

#define XENC_KEY "NyxOS_AUTH_v5.3"
#define XENC_KEY_LEN 15
#define MAX_FALLBACK_USERS 8

static uint32_t hash_with_salt(const char* str, const char* salt) {
    uint32_t h = 5381;
    if (salt)
        while (*salt) h = ((h << 5) + h) + (uint8_t)*salt++;
    if (str)
        while (*str) h = ((h << 5) + h) + (uint8_t)*str++;
    return h;
}

static void xor_buf(char* buf, uint32_t len) {
    for (uint32_t i = 0; i < len; i++)
        buf[i] ^= XENC_KEY[i % XENC_KEY_LEN];
}

static void format_entry(char* buf, uint32_t bufsz,
                         const char* user, const char* salt,
                         uint32_t hash) {
    snprintf(buf, bufsz, "%s:%s:%08x\n", user, salt, hash);
}

static int parse_entry(const char* buf,
                       char* user, uint32_t user_sz,
                       char* salt, uint32_t salt_sz,
                       uint32_t* hash) {
    const char* p = buf;
    int i = 0;
    while (*p && *p != ':' && i < (int)user_sz - 1) user[i++] = *p++;
    if (*p != ':') return -1;
    user[i] = '\0'; p++;
    i = 0;
    while (*p && *p != ':' && i < (int)salt_sz - 1) salt[i++] = *p++;
    if (*p != ':') return -1;
    salt[i] = '\0'; p++;
    *hash = 0;
    while (*p >= '0' && *p <= '9') { *hash = (*hash << 4) | (*p - '0'); p++; }
    while (*p >= 'a' && *p <= 'f') { *hash = (*hash << 4) | (*p - 'a' + 10); p++; }
    while (*p >= 'A' && *p <= 'F') { *hash = (*hash << 4) | (*p - 'A' + 10); p++; }
    return 0;
}

/* ------------------------------------------------------------------ */
/*  Fallback: in-memory user table when no EXT2                       */
/* ------------------------------------------------------------------ */
typedef struct {
    char username[AUTH_MAX_USER];
    char salt[8];
    uint32_t hash;
} fallback_user_t;

static fallback_user_t fb_users[MAX_FALLBACK_USERS];
static int fb_count = 0;

static void add_fallback(const char* user, const char* pass) {
    if (fb_count >= MAX_FALLBACK_USERS) return;
    fallback_user_t* u = &fb_users[fb_count++];
    uint32_t slen = strlen(user);
    for (int i = 0; i < 7; i++)
        u->salt[i] = "0123456789abcdef"[(uint8_t)user[i % slen] & 0xF];
    u->salt[7] = '\0';
    u->hash = hash_with_salt(pass, u->salt);
    strncpy(u->username, user, AUTH_MAX_USER - 1);
    u->username[AUTH_MAX_USER - 1] = '\0';
}

static int fallback_verify(const char* user, const char* pass) {
    for (int i = 0; i < fb_count; i++) {
        if (strcmp(fb_users[i].username, user) == 0) {
            uint32_t h = hash_with_salt(pass, fb_users[i].salt);
            return (h == fb_users[i].hash) ? 1 : 0;
        }
    }
    return 0;
}

/* fallback_add: add a new fallback user at runtime */
static int fallback_add(const char* user, const char* pass) {
    if (fb_count >= MAX_FALLBACK_USERS) return -1;
    add_fallback(user, pass);
    return 0;
}

/* ------------------------------------------------------------------ */
/*  EXT2 passwd file helpers (XOR-encrypted)                          */
/* ------------------------------------------------------------------ */
static int passwd_exists(void) {
    if (ext2_fs.block_size == 0) return 0;
    return ext2_resolve(AUTH_PATH) != 0;
}

static int read_passwd(char* buf, uint32_t sz) {
    if (ext2_fs.block_size == 0) return -1;
    int r = ext2_read_file(AUTH_PATH, buf, sz);
    if (r > 0) xor_buf(buf, r);
    return r;
}

static int write_passwd(const char* buf, uint32_t len) {
    if (ext2_fs.block_size == 0) return -1;
    char tmp[2048];
    uint32_t cplen = len < sizeof(tmp) ? len : sizeof(tmp) - 1;
    memcpy(tmp, buf, cplen);
    xor_buf(tmp, cplen);
    ext2_create_file(AUTH_PATH);
    return ext2_write_file(AUTH_PATH, tmp, cplen);
}

/* ------------------------------------------------------------------ */
/*  Public API                                                        */
/* ------------------------------------------------------------------ */
int auth_setup(void) {
    if (ext2_fs.block_size == 0 || !ext2_resolve(AUTH_PATH)) {
        add_fallback("nyx", "nyx");
        add_fallback("root", "root");
        add_fallback("admin", "admin");
        printf("[AUTH] EXT2 not available — %d fallback user(s) loaded\n", fb_count);
        return 0;
    }

    if (!passwd_exists()) {
        const char* default_user = "nyx";
        const char* default_pass = "nyx";
        const char* default_salt = "nx";
        uint32_t h = hash_with_salt(default_pass, default_salt);
        char entry[128];
        format_entry(entry, sizeof(entry), default_user, default_salt, h);

        if (write_passwd(entry, strlen(entry)) > 0) {
            printf("[AUTH] Created default user 'nyx' (encrypted)\n");
            return 0;
        }
    } else {
        printf("[AUTH] Encrypted user file found at %s\n", AUTH_PATH);
        return 0;
    }
    return -1;
}

int auth_verify(const char* username, const char* password) {
    if (!username || !password) return 0;

    if (ext2_fs.block_size == 0 || !passwd_exists()) {
        return fallback_verify(username, password);
    }

    char buf[2048];
    int len = read_passwd(buf, sizeof(buf) - 1);
    if (len < 0) {
        return fallback_verify(username, password);
    }
    buf[len] = '\0';

    char line[256];
    int li = 0;
    for (int i = 0; i <= len; i++) {
        if (buf[i] == '\n' || buf[i] == '\0') {
            line[li] = '\0';
            if (li > 0) {
                char file_user[AUTH_MAX_USER];
                char file_salt[16];
                uint32_t file_hash;
                if (parse_entry(line, file_user, sizeof(file_user),
                                file_salt, sizeof(file_salt), &file_hash) == 0) {
                    if (strcmp(file_user, username) == 0) {
                        uint32_t check = hash_with_salt(password, file_salt);
                        if (check == file_hash) return 1;
                    }
                }
            }
            li = 0;
        } else {
            if (li < 255) line[li++] = buf[i];
        }
    }

    return fallback_verify(username, password);
}

void auth_add_user(const char* username, const char* password) {
    if (!username || !password) return;

    if (ext2_fs.block_size == 0 || !passwd_exists()) {
        if (fallback_add(username, password) == 0)
            printf("[AUTH] Added fallback user '%s' (hashed, %d users total)\n",
                   username, fb_count);
        return;
    }

    char existing[2048];
    int exlen = read_passwd(existing, sizeof(existing) - 1);
    if (exlen < 0) exlen = 0;
    existing[exlen] = '\0';

    char salt[16];
    uint32_t slen = strlen(username);
    for (int i = 0; i < 7; i++)
        salt[i] = "0123456789abcdef"[(uint8_t)username[i % slen] & 0xF];
    salt[7] = '\0';

    uint32_t h = hash_with_salt(password, salt);
    char new_entry[128];
    format_entry(new_entry, sizeof(new_entry), username, salt, h);

    char combined[2048];
    snprintf(combined, sizeof(combined), "%s%s", existing, new_entry);
    if (write_passwd(combined, strlen(combined)) > 0)
        printf("[AUTH] Added user '%s' to encrypted passwd\n", username);
}
