#include <payload_dylib/common.h>
#include <payload_dylib/crashreporter.h>
#include <spawn.h>
#include <string.h>
#include <mach-o/dyld.h>

int posix_spawnp_orig_wrapper(pid_t *pid,
                              const char *path,
                              const posix_spawn_file_actions_t *action,
                              const posix_spawnattr_t *attr,
                              char *const argv[], char *envp[])
{
    int ret;
    pid_t current_pid = getpid();
    if (current_pid == 1) crashreporter_pause();
    ret = posix_spawnp(pid, path, action, attr, argv, envp);
    if (current_pid == 1) crashreporter_resume();
    return ret;
}

int hook_posix_spawnp_launchd(pid_t *pid,
                              const char *path,
                              const posix_spawn_file_actions_t *action,
                              const posix_spawnattr_t *attr,
                              char *const argv[], char *envp[])
{
    if ((pflags & palerain_option_rootful) == 0 &&
        strcmp(path, "/System/Library/CoreServices/SpringBoard.app/SpringBoard") 
        && (argv[1] == NULL || strcmp(argv[1], "com.apple.cfprefsd.xpc.daemon") || strcmp(argv[1], "com.apple.lsd.xpc.daemon"))
        ) {
        return posix_spawnp_orig_wrapper(pid, path, action, attr, argv, envp);
    }
    char *inj = NULL;
    int envcnt = 0;
    while (envp[envcnt] != NULL)
    {
        envcnt++;
    }
    char **newenvp = malloc((envcnt + 2) * sizeof(char **));
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
    if (currentenv)
    {
        size_t inj_len = strlen(currentenv) + 1 + strlen(newlib) + 1;
        inj = malloc(inj_len);
        if (inj == NULL)
        {
            perror(NULL);
            abort();
        }
        snprintf(inj, inj_len, "%s:%s", currentenv, newlib);
    }
    else
    {
        size_t inj_len = strlen("DYLD_INSERT_LIBRARIES=") + strlen(newlib) + 1;
        inj = malloc(inj_len);
        if (inj == NULL)
        {
            perror(NULL);
            abort();
        }
        snprintf(inj, inj_len, "DYLD_INSERT_LIBRARIES=%s", newlib);
    }
    newenvp[j] = inj;
    newenvp[j + 1] = NULL;

    int ret = posix_spawnp_orig_wrapper(pid, path, action, attr, argv, newenvp);
    if (inj != NULL)
        free(inj);
    if (currentenv != NULL)
        free(currentenv);
    return ret;
}

DYLD_INTERPOSE(hook_posix_spawnp_launchd, posix_spawnp);
