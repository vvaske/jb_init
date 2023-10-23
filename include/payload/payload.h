#ifndef PAYLOAD_PAYLOAD_H
#define PAYLOAD_PAYLOAD_H

#include <inttypes.h>
#include <stdbool.h>
#include <paleinfo.h>
#include <unistd.h>

#define CHECK_ERROR(action, loop, msg) do { \
 {int ___CHECK_ERROR_ret = action; \
 if (___CHECK_ERROR_ret) { \
  fprintf(stderr, msg ": %d (%s)\n", errno, strerror(errno)); \
  if (loop) spin(); \
 }} \
} while (0)

_Noreturn void spin();
int loader_main(int argc, char* argv[]);
int p1ctl_main(int argc, char* argv[]);
int attach_dmg(const char *source, bool is_overlay, char* device_path, size_t device_path_len);
int prelaunchd(uint32_t payload_options, uint64_t pflags);
int launchdaemons(uint32_t payload_options, uint64_t pflags);
int sysstatuscheck(uint32_t payload_options, uint64_t pflags);
int get_pinfo(struct paleinfo* pinfo_p);
int set_pinfo(struct paleinfo* pinfo_p);

enum {
    /* only for sysstatuscheck and prelaunchd stage! */
    payload_option_userspace_rebooted = UINT32_C(1) << 0,
    payload_option_launchdaemons = UINT32_C(1) << 0,
    payload_option_sysstatuscheck = UINT32_C(1) << 0,
    payload_option_prelaunchd = UINT32_C(1) << 0,
};

#endif
