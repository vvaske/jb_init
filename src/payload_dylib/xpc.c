#include <payload_dylib/common.h>
#include <string.h>
#include <sys/stat.h>
#include <xpc/private.h>

xpc_object_t hook_xpc_dictionary_get_value(xpc_object_t dict, const char *key){
  xpc_object_t retval = xpc_dictionary_get_value(dict,key);
  if (getpid() != 1) return retval;
  static int platform = 0;
  if (!platform) {
    platform = get_platform();
    int console_fd = open("/dev/console", O_RDWR | O_CLOEXEC);
    if (console_fd != -1) {
      dup2(console_fd, STDIN_FILENO);
      dup2(console_fd, STDOUT_FILENO);
      dup2(console_fd, STDERR_FILENO);
    }
#define fd_console STDOUT_FILENO
    if (platform == -1) spin();
#undef fd_console
  }
  if (strcmp(key,"LaunchDaemons") == 0) {
    xpc_object_t submitJob = xpc_dictionary_create(NULL, NULL, 0);
    xpc_object_t programArguments = xpc_array_create(NULL, 0);
    xpc_array_append_value(programArguments, xpc_string_create("/cores/payload"));
    if(getenv("XPC_USERSPACE_REBOOTED") != NULL) {
      xpc_array_append_value(programArguments, xpc_string_create("-u"));
    }
    xpc_array_append_value(programArguments, xpc_string_create("-j"));
    xpc_dictionary_set_bool(submitJob, "KeepAlive", false);
    xpc_dictionary_set_bool(submitJob, "RunAtLoad", true);
    xpc_dictionary_set_string(submitJob, "ProcessType", "Interactive");
    xpc_dictionary_set_string(submitJob, "UserName", "root");
    xpc_dictionary_set_string(submitJob, "Program", "/cores/payload");
    xpc_dictionary_set_string(submitJob, "StandardInPath", "/dev/console");
    xpc_dictionary_set_string(submitJob, "StandardOutPath", "/dev/console");
    xpc_dictionary_set_string(submitJob, "StandardErrorPath", "/dev/console");
    xpc_dictionary_set_string(submitJob, "Label", "lol.nickchan.payload");
    xpc_dictionary_set_value(submitJob, "ProgramArguments", programArguments);
    xpc_dictionary_set_value(retval, "/System/Library/LaunchDaemons/lol.nickchan.payload.plist", submitJob);

    char dropbearBuf[150];
    struct stat st;
    snprintf(dropbearBuf, 150, "/cores/binpack/Library/LaunchDaemons/dropbear-%d.plist", platform);
    printf("dropbearBuf: %s\n", dropbearBuf);
    int dropbear_fd = open(dropbearBuf, O_RDONLY);
    if (dropbear_fd == -1) return retval;
    if (fstat(dropbear_fd, &st)) {
      close(dropbear_fd);
      return retval;
    }
    void* addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, dropbear_fd, 0);
    if (addr != MAP_FAILED) {
      printf("map dropbear plist %s successed\n", dropbearBuf);
      xpc_object_t dropbearDict = xpc_create_from_plist(addr, st.st_size);
      printf("dropbearDict: %p\n", dropbearDict);
      /* actual plist path does not work ??? */
      if (dropbearDict) xpc_dictionary_set_value(retval, "/System/Library/LaunchDaemons/com.mkj.dropbear.plist", dropbearDict);
    }
    munmap(addr, st.st_size);
    close(dropbear_fd);
  } else if (strcmp(key, "sysstatuscheck") == 0) {
    xpc_object_t programArguments = xpc_array_create(NULL, 0);
    xpc_array_append_value(programArguments, xpc_string_create("/cores/payload"));
    if(getenv("XPC_USERSPACE_REBOOTED") != NULL) {
      xpc_array_append_value(programArguments, xpc_string_create("-u"));
    }
    xpc_array_append_value(programArguments, xpc_string_create("-s"));
    xpc_object_t newTask = xpc_dictionary_create(NULL, NULL, 0);
    xpc_dictionary_set_bool(newTask, "PerformAfterUserspaceReboot", true);
    xpc_dictionary_set_bool(newTask, "RebootOnSuccess", true);
    xpc_dictionary_set_string(newTask, "Program", "/cores/payload");
    xpc_dictionary_set_value(newTask, "ProgramArguments", programArguments);
    return newTask;
  } else if (strcmp(key, "Paths") == 0) {
    if ((pflags & palerain_option_safemode) == 0) {
      if (pflags & palerain_option_rootful)
        xpc_array_append_value(retval, xpc_string_create("/Library/LaunchDaemons"));
      else {
        xpc_array_append_value(retval, xpc_string_create("/var/jb/Library/LaunchDaemons"));
        xpc_array_append_value(retval, xpc_string_create("/var/jb/System/Library/LaunchDaemons"));
      }
    }
  }
  return retval;
}
DYLD_INTERPOSE(hook_xpc_dictionary_get_value, xpc_dictionary_get_value);

bool hook_xpc_dictionary_get_bool(xpc_object_t dictionary, const char *key) {
  if (!strcmp(key, "LogPerformanceStatistics")) return true;
  else return xpc_dictionary_get_bool(dictionary, key);
}
DYLD_INTERPOSE(hook_xpc_dictionary_get_bool, xpc_dictionary_get_bool);
