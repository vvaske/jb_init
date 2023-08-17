#include <fakedyld/fakedyld.h>
#include <mount_args.h>

#ifdef ASAN
#define CORES_SIZE 8388608
#else
#define CORES_SIZE 2097152
#endif

void mount_cores14() {
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

void mount_early_xarts_cores() {
    
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

void mount_var_on_cores() {
  int ret;
  pid_t pid;
  ret = posix_spawn(&pid, "/usr/libexec/init_data_protection", NULL, ((char*[]){ "/usr/libexec/init_data_protection", NULL }), NULL);
  if (ret != 0) {
    LOG("posix_spawn /usr/libexec/init_data_protection failed: %d\n", ret);
    spin();
  }
  int child_stat;
  wait4(pid, &child_stat, 0, NULL);
  apfs_mountarg_t var_cores_arg = { "/dev/disk0s1s2 "};
  ret = mount("apfs", "/cores", 0, &var_cores_arg);
  if (ret) {
    LOG("mount var onto /cores failed: %d", ret);
    spin();
  }
}

void init_cores(struct systeminfo* sysinfo_p) {
  if (sysinfo_p->osrelease.darwinMajor < 19) {
    mount_var_on_cores();
  } else {
    mount_cores14();
  }
  cores_mkdir("/cores/binpack");
  cores_mkdir("/cores/fs");
  cores_mkdir("/cores/fs/real");
  cores_mkdir("/cores/fs/fake");
}
