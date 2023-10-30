#ifndef _MOUNT_ARGS_H
#define _MOUNT_ARGS_H
#include <stdint.h>
#if !__STDC_HOSTED__
#include <fakedyld/types.h>
#else
#include <sys/time.h>
#endif

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

#define HFSFSMNT_NOXONFILES	0x1	/* disable execute permissions for files */
#define HFSFSMNT_WRAPPER	0x2	/* mount HFS wrapper (if it exists) */
#define HFSFSMNT_EXTENDED_ARGS  0x4     /* indicates new fields after "flags" are valid */

/* Fourth argument to mount(2) when mounting apfs */
typedef struct apfs_mount_args {
    char* path; /* path to device to mount from */
    uint64_t _null; /* 0 */
    uint32_t mount_mode; /* see above define */
    uint32_t _pad; /* 0 */
    char snapshot[0x100]; /* snapshot name when mount mode is 2 */
    char im4p[16]; /* some representation of root hash im4p */
    char im4m[16]; /* some representation of root hash im4m */
} apfs_mount_args_t;

/*
 * Fourth argument to mount(2) when mounting hfs 
*/
typedef struct hfs_mount_args {
	char	*fspec;			/* block special device to mount */
	uid_t	hfs_uid;		/* uid that owns hfs files (standard HFS only) */
	gid_t	hfs_gid;		/* gid that owns hfs files (standard HFS only) */
	mode_t	hfs_mask;		/* mask to be applied for hfs perms  (standard HFS only) */
	uint32_t hfs_encoding;	/* encoding for this volume (standard HFS only) */
	struct	timezone hfs_timezone;	/* user time zone info (standard HFS only) */
	int		flags;			/* mounting flags, see below */
	int     journal_tbuffer_size;   /* size in bytes of the journal transaction buffer */
	int		journal_flags;          /* flags to pass to journal_open/create */
	int		journal_disable;        /* don't use journaling (potentially dangerous) */

} hfs_mount_args_t;

/* Fourth argument to mount(2) when mounting tmpfs */
typedef struct tmpfs_mountarg {
    uint64_t max_pages; /* maximum amount of memory pages to be used for this tmpfs*/
    uint64_t max_nodes; /* maximum amount of inodes in this tmpfs */
    uint8_t case_insensitive; /* 1 = case insensitive, 0 = case sensitive */
} tmpfs_mountarg_t;

#endif /* MOUNR_ARGS_H */
