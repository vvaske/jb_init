#ifndef MOUNT_ARGS_H
#define MOUNT_ARGS_H
#include <stdint.h>

/* APFS mode modes */
#define APFS_MOUNT_DEFAULT_SNAPSHOT         0x0 /* mount the default snapshot */
#define APFS_MOUNT_LIVEFS                   0x1 /* mount live fs */
#define APFS_MOUNT_CUSTOM_SNAPSHOT          0x2 /* mount custom snapshot in apfs_mountarg.snapshot */
#define APFS_IMG4_SNAPSHOT                  0x3 /* mount snapshot while suppling some representation of im4p and im4m */
/* 
 * mount modes set by mount_apfs when -C and -c are used respectively.
 * Possibly related to fusion drives.
 */
#define APFS_MOUNT_MNTAPFS_LARGE_C_OPT      0x4 /* Set by mount_apfs when -C is used */
#define APFS_MOUNT_MNTAPFS_SMALL_C_OPT      0x5 /* Set by mount_apfs when -c is used */
#define APFS_MOUNT_IMG4                     0x8 /* mount live fs while suppling some representation of im4p and im4m */
/* End APFS mount modes*/

/* Fourth argument to mount(2) when mounting apfs */
typedef struct apfs_mountarg {
    char* path; /* path to device to mount from */
    uint64_t _null; /* 0 */
    uint32_t mount_mode; /* see above define */
    uint32_t _pad; /* 0 */
    char snapshot[0x100]; /* snapshot name when mount mode is 2 */
    char im4p[16]; /* some representation of root hash im4p */
    char im4m[16]; /* some representation of root hash im4m */
} apfs_mountarg_t;

/*
 * Fourth argument to mount(2) when mounting hfs 
 * string representing the device to mount from
*/
typedef char* hfs_mountarg_t;

/* Fourth argument to mount(2) when mounting tmpfs */
typedef struct tmpfs_mountarg {
    uint64_t max_pages; /* maximum amount of memory pages to be used for this tmpfs*/
    uint64_t max_nodes; /* maximum amount of inodes in this tmpfs */
    uint8_t case_insensitive; /* 1 = case insensitive, 0 = case sensitive */
} tmpfs_mountarg_t;

#endif /* MOUNR_ARGS_H */
