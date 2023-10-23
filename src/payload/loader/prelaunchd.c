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

int prelaunchd(uint32_t payload_options, uint64_t pflags) {
    char binpack_device[50], loader_device[50];

    CHECK_ERROR(attach_dmg("ramfile://checkra1n", 1, binpack_device, 50), 1, "attach binpack failed");
    CHECK_ERROR(mount("hfs", "/cores/binpack", MNT_RDONLY, binpack_device), 1, "mount binpack failed");
    CHECK_ERROR(attach_dmg("/cores/binpack/loader.dmg", 1, loader_device, 50), 1, "attach loader failed");
    CHECK_ERROR(mount("hfs", "/cores/binpack/Applications", MNT_RDONLY, loader_device), 1, "mount loader failed");

    return 0;
}
