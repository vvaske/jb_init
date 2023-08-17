#ifndef FAKEDYLD_H
#define FAKEDYLD_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <fakedyld/startup.h>
#include <fakedyld/types.h>
#include <fakedyld/syscalls.h>
#include <fakedyld/errno.h>
#include <fakedyld/param.h>
#include <fakedyld/printf.h>
#include <fakedyld/lib.h>
#include <fakedyld/string.h>
#include <fakedyld/utils.h>
#include <fakedyld/rw_file.h>
#include <fakedyld/spawn.h>
#include <paleinfo.h>

void mountroot(struct paleinfo* pinfo_p, struct systeminfo* sysinfo_p);
void init_cores(struct systeminfo* sysinfo_p);
void get_and_patch_dyld(void);
void prepare_rootfs(struct systeminfo* sysinfo_p, struct paleinfo* pinfo_p);
void systeminfo(struct systeminfo* sysinfo_p);

#endif
