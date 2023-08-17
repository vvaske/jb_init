#ifndef FAKEDYLD_STARTUP_H
#define FAKEDYLD_STARTUP_H

#include <stdint.h>

#if 0
#define MAX_KERNEL_ARGS 128

/* how kernel passes argc, argv, envp, apple etc */
struct KernelArgs {
    const void* mainExecutable;
    uintptr_t argc;
    /* basically argv, envp and apple squished together */
    const char* args[MAX_KERNEL_ARGS];
};
#endif

int main(void);

#endif
