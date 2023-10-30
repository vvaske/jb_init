#include <stdio.h>
#include <spawn.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <mount_args.h>

int attach_dmg(const char *source, char* device_path, size_t device_path_len);

_Noreturn void spin() {
    fprintf(stderr, "an error occured\n");
    while(1) {
        sleep(3600);
    }
}

int main() {
    if (getppid() != 1) {
        fprintf(stderr, "this is a plooshra1n internal utility, do not use\n");
        return -1;
    }
    char device_path[50];
    int ret = attach_dmg("ram://1536", device_path, 50);
    if (ret) spin();
    pid_t pid;
    ret = posix_spawn(&pid, "/sbin/newfs_hfs", NULL, NULL, (char*[]){ "/sbin/newfs_hfs", "-s", "-v", "nebula is a furry", device_path, NULL } , NULL);
    if (ret) {
        fprintf(stderr, "faild to spawn /sbin/newfs_hfs: %d\n", errno);
        spin();
    }
    int status;
    waitpid(pid, &status, 0);
    if (!WIFEXITED(status) || WEXITSTATUS(status)) {
        fprintf(stderr, "/sbin/newfs_hfs failed\n");
        spin();
    }
    struct hfs_mount_args cores_mountarg = { device_path, 0, 0, 0, 0, { 0, 0 }, HFSFSMNT_EXTENDED_ARGS, 0, 0, 1 };
    ret = mount("hfs", "/cores", 0, &cores_mountarg);
    if (ret) {
        fprintf(stderr, "mount failed: %d\n", errno);
        spin();
    }
    return 0;
}
