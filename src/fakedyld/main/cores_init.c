#include <fakedyld/fakedyld.h>
#include <mount_args.h>

#ifdef ASAN
#define CORES_SIZE 8388608
#else
#define CORES_SIZE 2097152
#endif

void mount_tmpfs_cores() {
    int err = 0;
    int64_t pagesize;
    unsigned long pagesize_len = sizeof(pagesize);
    err = sys_sysctlbyname("hw.pagesize", sizeof("hw.pagesize"), &pagesize, &pagesize_len, NULL, 0);
    if (err != 0)
    {
        LOG("cannot get pagesize, err=%d\n", err);
        spin();
    }
    LOG("system page size: %lld\n", pagesize);
    {
        struct tmpfs_mountarg arg = {.max_pages = (CORES_SIZE / pagesize), .max_nodes = UINT8_MAX, .case_insensitive = 0};
        err = mount("tmpfs", "/cores", 0, &arg);
        if (err != 0)
        {
          LOG("cannot mount tmpfs onto /cores, err=%d\n", err);
          spin();
        }
        LOG("mounted tmpfs onto /cores\n");
    }
}

void cores_mkdir(char* path) {
    struct stat statbuf;
    int err = mkdir(path, 0755);
    if (err) {
        LOG("mkdir(%s) FAILED with err %d\n", path, err);
    }
    if (stat(path, &statbuf)) {
        LOG("stat %s FAILED with err=%d!\n", path, err);
        spin();
    }
    else
        LOG("created %s\n", path);
}

void mount_ramdisk_cores(int platform) {
  char executable[30];
  switch (platform) {
    case PLATFORM_IOS:
    case PLATFORM_TVOS:
    case PLATFORM_BRIDGEOS:
      snprintf(executable, 30, "/mount_cores.%d", platform);
      break;
    default:
      LOG("mount_cores: unsupported platform");
      spin();
  }
  pid_t pid;
  int ret = posix_spawn(&pid, executable, NULL, NULL, NULL);
  if (ret != 0) {
    LOG("failed to spawn %s: %d", executable, ret);
    spin();
  }
  wait4(pid, NULL, 0, NULL);
}

void init_cores(struct systeminfo* sysinfo_p, int platform) {
  if (sysinfo_p->osrelease.darwinMajor < 19) {
    mount_ramdisk_cores(platform);
  } else {
    mount_tmpfs_cores();
  }
  cores_mkdir("/cores/binpack");
  cores_mkdir("/cores/fs");
  cores_mkdir("/cores/fs/real");
  cores_mkdir("/cores/fs/fake");
}
