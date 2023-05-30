#include <fakedyld/fakedyld.h>
#include <mount_args.h>

/* Find and mount rootfs */
void mountroot(struct paleinfo* pinfo_p, struct systeminfo* sysinfo_p) {
    char dev_rootdev[32];
    if (pinfo_p->rootdev[0] != '\0' /*&& (pinfo_p->flags & palerain_option_setup_rootful) == 0*/) {
        snprintf(dev_rootdev, 32, "/dev/%s", pinfo_p->rootdev);
    } else if (sysinfo_p->osrelease.darwinMajor < 22) {
        snprintf(dev_rootdev, 32, "/dev/%s", DARWIN21_ROOTDEV);
    } else {
        snprintf(dev_rootdev, 32, "/dev/%s", DARWIN22_ROOTDEV);
    }
    struct stat st;
    int ret = stat(dev_rootdev, &st);
    if (ret == ENOENT && (pinfo_p->flags & palerain_option_rootful)) {
        LOG("fakefs does not exist! making one for you");
        spin();
    }
    if (pinfo_p->flags & palerain_option_test1) {
        LOG("testing chroot");
        int fd_root = open("/", O_DIRECTORY | O_RDONLY, 0);
        if (fd_root == -1) {
            LOG("could not open root fd\n");
            spin();
        }
        chroot("/mnt");
        chdir("/");
        ret = stat("/usr/lib/dyld", &st);
        if (ret != ENOENT) {
            LOG("stat(): %d (%s)\n", errno ,strerror(errno));
            spin();
        }
        LOG("stat() failed with error 2 (expected)\n");
        LOG("trying to get back out...");
        ret = fchdir(fd_root);
        if (ret) {
            LOG("fchdir() failed: %d (%s)\n", errno, strerror(errno));
            spin();
        }
        ret = stat("/usr/lib/dyld", &st);
        if (ret) {
            LOG("stat /usr/lib/dyld failed: %d (%s)", errno, strerror(errno));
            spin();
        }
        LOG("chroot test successful!");
        close(fd_root);
    }

    uint32_t mount_mode = APFS_MOUNT_DEFAULT_SNAPSHOT;
    if (pinfo_p->flags & palerain_option_rootful) 
        mount_mode = APFS_MOUNT_LIVEFS;
    struct apfs_mountarg arg = {
        dev_rootdev, 0, mount_mode, 0, "", "", ""
    };
    CHECK_ERROR(mount("apfs", "/", MNT_RDONLY, &arg), "mount rootfs %s failed", dev_rootdev);
    LOG("mount rootfs succeeded!");
    spin();
    

}
