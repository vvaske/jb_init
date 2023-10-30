#define __APPLE_API_PRIVATE

#include <stdio.h>
#include <paleinfo.h>
#include <xpc/xpc.h>
#include <stdint.h>
#include <spawn.h>
#include <errno.h>
#include <string.h>
#include <sys/reboot.h>
#include <sys/sysctl.h>
#include <sys/mount.h>
#include <payload_dylib/common.h>
#include <payload_dylib/crashreporter.h>

uint64_t pflags;

void _spin(int fd_console) {
    dprintf(fd_console, "An error occured");
}

uint64_t load_pflags(int fd_console) {
  int flides[2];
  int ret;
  CHECK_ERROR(pipe(flides), "pipe failed");
  posix_spawn_file_actions_t actions;
  posix_spawn_file_actions_init(&actions);
  posix_spawn_file_actions_adddup2(&actions, flides[1], STDOUT_FILENO);
  posix_spawn_file_actions_addopen(&actions, STDERR_FILENO, "/dev/console", O_WRONLY, 0);
  pid_t pid;
  CHECK_ERROR(posix_spawnp(&pid, "/cores/payload", &actions, NULL, (char*[]){"p1ctl","palera1n_flags","get",NULL}, NULL), "could not spawn p1ctl");
  ssize_t didRead;
  int status;
  char p1flags_buf[32];
  posix_spawn_file_actions_destroy(&actions);
  /* in principle the child may block indefinitely without read(), so we read() then waitpid() */
  didRead = read(flides[0], p1flags_buf, 31);
  if (didRead < 0) {
    dprintf(fd_console, "read failed: %d (%s)\n", errno, strerror(errno));
    spin();
  }
  waitpid(pid, &status, 0);
  close(flides[0]);
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    dprintf(fd_console, "p1ctl waitpid status: %d\n", status);
    spin();
  }
  p1flags_buf[31] = '\0';
  dprintf(fd_console, "p1ctl output: %s", p1flags_buf);
  pflags = (uint64_t)strtoull(p1flags_buf, NULL, 16);
  dprintf(fd_console, "pflags: %llu\n", pflags);
  return pflags;
}

__attribute__((constructor))void launchd_hook_main() {
  if (getpid() != 1) return;
  int fd_console = open("/dev/console",O_RDWR|O_SYNC,0);
  if (fd_console == -1) {
    char errMsg[1024];
    snprintf(errMsg, 1024, "payload.dylib cannot open /dev/console: %d (%s)", errno, strerror(errno));
    reboot_np(RB_PANIC, errMsg);
  }
  dprintf(fd_console, "=========== hello from payload.dylib ===========\n");
  crashreporter_start();
  load_pflags(fd_console);
  pid_t pid;
  posix_spawn_file_actions_t actions;
  posix_spawn_file_actions_init(&actions);
  posix_spawn_file_actions_addopen(&actions, STDIN_FILENO, "/dev/console", O_RDWR, 0);
  posix_spawn_file_actions_addopen(&actions, STDOUT_FILENO, "/dev/console", O_WRONLY, 0);
  posix_spawn_file_actions_addopen(&actions, STDERR_FILENO, "/dev/console", O_WRONLY, 0);
  int ret, status;
  CHECK_ERROR(posix_spawn(&pid, "/cores/payload", &actions, NULL, (char*[]){"/cores/payload","-f",NULL},environ), "could not spawn payload");
  posix_spawn_file_actions_destroy(&actions);
  waitpid(pid, &status, 0);
  if (WIFEXITED(status)) {
    if (WEXITSTATUS(status) != 0) {
      dprintf(fd_console, "/cores/payload exited with status code %d\n", WEXITSTATUS(status));
      spin();
    }
  } else if (WIFSIGNALED(status)) {
    dprintf(fd_console, "/cores/payload exited abnormally: signal %d\n", WTERMSIG(status));
    spin();
  } else {
    spin();
  }
  if ((pflags & palerain_option_setup_rootful)) {
    int32_t initproc_started = 1;
    CHECK_ERROR(sysctlbyname("kern.initproc_spawned", NULL, NULL, &initproc_started, 4), "sysctl kern.initproc_spawned=1");
    CHECK_ERROR(unmount("/cores/binpack/Applications", MNT_FORCE), "unmount(/cores/binpack/Applications)");
    CHECK_ERROR(unmount("/cores/binpack", MNT_FORCE), "unmount(/cores/binpack)");
    dprintf(fd_console, "Rebooting\n");
    reboot_np(RB_AUTOBOOT, NULL);
    sleep(5);
    dprintf(fd_console, "reboot timed out\n");
    spin();
  }
  close(fd_console);
}
