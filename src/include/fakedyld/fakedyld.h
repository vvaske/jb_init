#ifndef FAKEDYLD_FAKEDYLD_H
#define FAKEDYLD_FAKEDYLD_H

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
#include <paleinfo.h>

void systeminfo(struct systeminfo* sysinfo_p);
void rootwait(struct systeminfo* sysinfo_p);
void chroot_realfs(struct paleinfo* pinfo_p, struct systeminfo* sysinfo_p);
void mountroot(struct paleinfo* pinfo_p, struct systeminfo* sysinfo_p);
#endif
