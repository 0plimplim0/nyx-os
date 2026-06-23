#include "kernel.h"
#include "ata.h"
#include "ext2.h"

ext2_fs_t ext2_fs;
static uint8_t sector_buf[2048];

static void read_sectors(uint32_t lba, uint8_t count, void* buf) {
    ata_read_sectors(ext2_fs.drive, ext2_fs.part_start_lba + lba, count, buf);
}

static uint32_t block_to_lba(uint32_t block) {
    uint32_t block_size = ext2_fs.block_size;
    uint32_t sectors_per_block = block_size / 512;
    return block * sectors_per_block;
}

static uint32_t inode_to_block(uint32_t ino) {
    uint32_t group = (ino - 1) / ext2_fs.inodes_per_group;
    uint32_t index = (ino - 1) % ext2_fs.inodes_per_group;

    ext2_bgd_t bgd;
    uint32_t bgd_lba = block_to_lba(ext2_fs.bgd_block) + group * sizeof(ext2_bgd_t) / 512;
    read_sectors(bgd_lba, 1, sector_buf);
    uint32_t off = (group * sizeof(ext2_bgd_t)) % 512;
    __builtin_memcpy(&bgd, sector_buf + off, sizeof(ext2_bgd_t));

    uint32_t inode_table_block = bgd.inode_table;
    uint32_t bytes_per_inode = ext2_fs.inode_size;
    uint32_t inode_offset = index * bytes_per_inode;
    return inode_table_block + inode_offset / ext2_fs.block_size;
}

void init_ext2(void) {
    // EXT2 is mounted lazily via ext2ls/ext2cat commands
}

int ext2_mount(uint8_t drive, uint32_t part_lba) {
    ext2_fs.drive = drive;
    ext2_fs.part_start_lba = part_lba;

    read_sectors(2, 2, sector_buf);
    __builtin_memcpy(&ext2_fs.sb, sector_buf, sizeof(ext2_superblock_t));

    if (ext2_fs.sb.magic != EXT2_SUPER_MAGIC)
        return -1;

    ext2_fs.block_size = 1024 << ext2_fs.sb.log_block_size;
    ext2_fs.inodes_per_group = ext2_fs.sb.inodes_per_group;
    ext2_fs.blocks_per_group = ext2_fs.sb.blocks_per_group;
    ext2_fs.inode_size = ext2_fs.sb.inode_size ? ext2_fs.sb.inode_size : 128;

    uint32_t block_groups = (ext2_fs.sb.total_blocks + ext2_fs.blocks_per_group - 1) / ext2_fs.blocks_per_group;
    uint32_t bgd_size = block_groups * sizeof(ext2_bgd_t);
    ext2_fs.bgd_blocks = (bgd_size + ext2_fs.block_size - 1) / ext2_fs.block_size;

    if (ext2_fs.block_size == 1024)
        ext2_fs.bgd_block = 2;
    else
        ext2_fs.bgd_block = 1;

    return 0;
}

int ext2_read_inode(uint32_t ino, ext2_inode_t* inode) {
    uint32_t block = inode_to_block(ino);
    uint32_t index = (ino - 1) % ext2_fs.inodes_per_group;

    uint32_t sector_lba = block_to_lba(block);
    uint32_t offset_in_block = (index * ext2_fs.inode_size) % ext2_fs.block_size;
    uint32_t byte_offset = sector_lba * 512 + offset_in_block;

    uint32_t sector = byte_offset / 512;
    read_sectors(sector, (offset_in_block + sizeof(ext2_inode_t) + 511) / 512, sector_buf);

    uint32_t buf_off = byte_offset % 512;
    __builtin_memcpy(inode, sector_buf + buf_off, sizeof(ext2_inode_t));
    return 0;
}

int ext2_read_block(uint32_t block, void* buf) {
    uint32_t lba = block_to_lba(block);
    uint32_t sectors = ext2_fs.block_size / 512;
    for (uint32_t i = 0; i < sectors; i++)
        read_sectors(lba + i, 1, (uint8_t*)buf + i * 512);
    return 0;
}

int ext2_read_inode_block(ext2_inode_t* inode, uint32_t iblock, void* buf) {
    uint32_t ptrs_per_block = ext2_fs.block_size / 4;

    if (iblock < 12) {
        return ext2_read_block(inode->block[iblock], buf);
    }

    iblock -= 12;
    if (iblock < ptrs_per_block) {
        uint32_t indirect[1024 / 4];
        ext2_read_block(inode->block[12], indirect);
        return ext2_read_block(indirect[iblock], buf);
    }

    iblock -= ptrs_per_block;
    if (iblock < ptrs_per_block * ptrs_per_block) {
        uint32_t dindirect[1024 / 4];
        ext2_read_block(inode->block[13], dindirect);
        uint32_t block_idx = dindirect[iblock / ptrs_per_block];
        uint32_t indirect[1024 / 4];
        ext2_read_block(block_idx, indirect);
        return ext2_read_block(indirect[iblock % ptrs_per_block], buf);
    }

    return -1;
}

static uint32_t resolve_path(const char* path) {
    if (!path || path[0] != '/') return 0;
    if (path[1] == '\0') return EXT2_ROOT_INO;

    uint32_t cur_ino = EXT2_ROOT_INO;
    char component[256];
    const char* p = path + 1;

    while (*p) {
        int ci = 0;
        while (*p && *p != '/') component[ci++] = *p++;
        component[ci] = '\0';
        if (*p == '/') p++;

        if (ci == 0) continue;

        ext2_inode_t dir_inode;
        ext2_read_inode(cur_ino, &dir_inode);

        int found = 0;
        uint32_t bytes_left = dir_inode.size;
        uint32_t iblock = 0;

        while (bytes_left > 0 && !found) {
            uint8_t block_buf[1024];
            ext2_read_inode_block(&dir_inode, iblock, block_buf);
            uint32_t off = 0;
            while (off < ext2_fs.block_size && off < bytes_left) {
                ext2_dirent_t* de = (ext2_dirent_t*)(block_buf + off);
                if (de->inode == 0) { off += de->rec_len; continue; }
                char dname[256];
                __builtin_memcpy(dname, de->name, de->name_len);
                dname[de->name_len] = '\0';
                if (strcmp(dname, component) == 0) {
                    cur_ino = de->inode;
                    found = 1;
                    break;
                }
                off += de->rec_len;
            }
            bytes_left -= ext2_fs.block_size;
            iblock++;
        }

        if (!found) return 0;
    }

    return cur_ino;
}

int ext2_ls(const char* path) {
    uint32_t ino = resolve_path(path);
    if (!ino) {
        printf("ext2: path not found: %s\n", path);
        return -1;
    }

    ext2_inode_t inode;
    ext2_read_inode(ino, &inode);

    if (!(inode.mode & EXT2_S_IFDIR)) {
        printf("ext2: not a directory: %s\n", path);
        return -1;
    }

    printf("Directory listing: %s (inode %u)\n", path, ino);
    uint32_t bytes_left = inode.size;
    uint32_t iblock = 0;
    int count = 0;

    while (bytes_left > 0) {
        uint8_t block_buf[1024];
        if (ext2_read_inode_block(&inode, iblock, block_buf) < 0)
            break;
        uint32_t off = 0;
        while (off < ext2_fs.block_size && off < bytes_left) {
            ext2_dirent_t* de = (ext2_dirent_t*)(block_buf + off);
            if (de->inode == 0) { off += de->rec_len ? de->rec_len : 8; continue; }
            char dname[256];
            __builtin_memcpy(dname, de->name, de->name_len);
            dname[de->name_len] = '\0';
            printf("  %c %u %s\n",
                   (de->file_type == EXT2_FT_DIR) ? 'd' : 'f',
                   de->inode, dname);
            count++;
            off += de->rec_len;
        }
        bytes_left -= ext2_fs.block_size;
        iblock++;
    }
    printf("  (%d entries)\n", count);
    return 0;
}

int ext2_cat(const char* path) {
    uint32_t ino = resolve_path(path);
    if (!ino) {
        printf("ext2: path not found: %s\n", path);
        return -1;
    }

    ext2_inode_t inode;
    ext2_read_inode(ino, &inode);

    if (!(inode.mode & EXT2_S_IFREG)) {
        printf("ext2: not a file: %s\n", path);
        return -1;
    }

    uint32_t remaining = inode.size;
    uint32_t iblock = 0;
    while (remaining > 0) {
        uint8_t block_buf[1024];
        ext2_read_inode_block(&inode, iblock, block_buf);
        uint32_t chunk = (remaining < ext2_fs.block_size) ? remaining : ext2_fs.block_size;
        for (uint32_t i = 0; i < chunk; i++)
            putchar(block_buf[i]);
        remaining -= chunk;
        iblock++;
    }
    return 0;
}
