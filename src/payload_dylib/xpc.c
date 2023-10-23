#include <payload_dylib/common.h>
#include <string.h>

xpc_object_t hook_xpc_dictionary_get_value(xpc_object_t dict, const char *key){
  xpc_object_t retval = xpc_dictionary_get_value(dict,key);
  if (getpid() != 1) return retval;
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
