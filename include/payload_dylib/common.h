#ifndef PAYLOAD_DYLIB_CRASHREPORTER_H
#define PAYLOAD_DYLIB_CRASHREPORTER_H

#include <stdint.h>
#include <dyld-interpose.h>
#include <payload_dylib/crashreporter.h>
#include <paleinfo.h>
#include <xpc/xpc.h>

#define spin() _spin(fd_console)

#define CHECK_ERROR(action, msg) do { \
 ret = action; \
 if (ret) { \
  dprintf(fd_console, msg ": %d (%s)\n", errno, strerror(errno)); \
  spin(); \
 } \
} while (0)

extern uint64_t pflags;
extern char** environ;
void _spin(int fd_console);
int get_platform();

#endif
