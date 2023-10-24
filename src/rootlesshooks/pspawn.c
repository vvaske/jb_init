#include <stdlib.h>
#include <stdio.h>
#include <dyld-interpose.h>
#include <spawn.h>
#include <mach-o/dyld.h>
#include <substrate.h>
#include <string.h>

int hook_posix_spawnp(pid_t *pid,
                      const char *path,
                      const posix_spawn_file_actions_t *action,
                      const posix_spawnattr_t *attr,
                      char *const argv[], char *envp[]) {
    if (strcmp(argv[0], "/usr/sbin/cfprefsd") && strcmp(argv[0], "/usr/libexec/lsd"))
    {
        return posix_spawnp(pid, path, action, attr, argv, envp);
    }
    int envcnt = 0;
    while (envp[envcnt] != NULL)
    {
        envcnt++;
    }

    char **newenvp = malloc((envcnt + 2) * sizeof(char **));
    if (newenvp == NULL)
        abort();
    int j = 0;
    char *currentenv = NULL;
    for (int i = 0; i < envcnt; i++)
    {
        if (strstr(envp[j], "DYLD_INSERT_LIBRARIES") != NULL)
        {
            currentenv = envp[j];
            continue;
        }
        newenvp[i] = envp[j];
        j++;
    }

    char *newlib = "/cores/binpack/usr/lib/rootlesshooks.dylib";
    char *inj = NULL;
    if (currentenv)
    {
        size_t inj_len = strlen(currentenv) + 1 + strlen(newlib) + 1;
        inj = malloc(inj_len);
        if (inj == NULL)
            abort();
        snprintf(inj, inj_len, "%s:%s", currentenv, newlib);
    }
    else
    {
        size_t inj_len = strlen("DYLD_INSERT_LIBRARIES=") + strlen(newlib) + 1;
        inj = malloc(inj_len);
        if (inj == NULL)
            abort();
        snprintf(inj, inj_len, "DYLD_INSERT_LIBRARIES=%s", newlib);
    }
    newenvp[j] = inj;
    newenvp[j + 1] = NULL;

    int ret = posix_spawnp(pid, path, action, attr, argv, newenvp);
    free(inj);
    free(newenvp);
    return ret;
}

void xpcproxyInit(void) {
    void* hook_posix_spawnp_ptr = &hook_posix_spawnp;
    MSHookFunction(&posix_spawnp, &hook_posix_spawnp, &hook_posix_spawnp_ptr);
}
