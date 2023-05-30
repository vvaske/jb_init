#include <fakedyld/fakedyld.h>

void rootwait(struct systeminfo* sysinfo_p) {
    int ret;
    struct stat st;
    uint32_t i = 0;
    char dev_wait[32];
    if (sysinfo_p->osrelease.darwinMajor < 22) {
        snprintf(dev_wait, 32, "/dev/%s", DARWIN21_ROOTDEV);
    } else {
        snprintf(dev_wait, 32, "/dev/%s", DARWIN22_ROOTDEV);
    }
    while ((ret = stat(dev_wait, &st))) {
        i++;
        if (errno != ENOENT) {
            LOG("stat(%s) failed: %d (%s)", dev_wait, errno, strerror(errno));
            spin();
        }
        if (i % 20 == 0) {
            LOG("still waiting for root device");
        }
        sleep(1);
    }
}
