#include <payload/payload.h>
#include <paleinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/param.h>
#include <sys/types.h>
#include <limits.h>
#include <spawn.h>
#include <sys/mount.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <APFS/APFS.h>
#include <mount_args.h>

int prelaunchd(uint32_t payload_options, struct paleinfo* pinfo_p) {
    printf("plooshInit prelaunchd...\n");
    char binpack_device[50], loader_device[50];

    struct hfs_mount_args binpack_mountarg = { binpack_device, 0, 0, 0, 0, { 0, 0 }, HFSFSMNT_EXTENDED_ARGS, 0, 0, 1 };
    struct hfs_mount_args loader_mountarg = { loader_device, 0, 0, 0, 0, { 0, 0 }, HFSFSMNT_EXTENDED_ARGS, 0, 0, 1 };

    CHECK_ERROR(attach_dmg("ramfile://checkra1n", 1, binpack_device, 50), 1, "attach binpack failed");
    printf("========================\nbinpack_device: %s\n", binpack_device);
    usleep(500000);
    CHECK_ERROR(mount("hfs", "/cores/binpack", MNT_RDONLY, &binpack_mountarg), 1, "mount binpack failed");
    CHECK_ERROR(attach_dmg("/cores/binpack/loader.dmg", 0, loader_device, 50), 1, "attach loader failed");
    printf("========================\nloader_device: %s\n", loader_device);
    usleep(500000);
    CHECK_ERROR(mount("hfs", "/cores/binpack/Applications", MNT_RDONLY, &loader_mountarg), 1, "mount loader failed");

    
    char dev_rootdev[32];
    snprintf(dev_rootdev, 32, "/dev/%s", pinfo_p->rootdev);

    if ((pinfo_p->flags & palerain_option_rootful) && ((pinfo_p->flags & palerain_option_force_revert))) {
        if (access(dev_rootdev, F_OK) == 0)
            CHECK_ERROR(APFSVolumeDelete(pinfo_p->rootdev), 1, "failed to delete fakefs");
    }
    
    if (pinfo_p->flags & palerain_option_setup_rootful) {
        return setup_fakefs(payload_options, pinfo_p);
    }

    return 0;
}
