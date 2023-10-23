#include <payload_dylib/common.h>
#include <payload_dylib/crashreporter.h>
#include <spawn.h>
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
    if (argv[1] == NULL || strcmp(argv[1], "com.apple.cfprefsd.xpc.daemon"))
        return posix_spawnp_orig_wrapper(pid, path, action, attr, argv, envp);
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

    char *newlib = "/cores/payload.dylib";
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

int hook_posix_spawnp_xpcproxy(pid_t *pid,
                               const char *path,
                               const posix_spawn_file_actions_t *action,
                               const posix_spawnattr_t *attr,
                               char *const argv[], char *envp[])
{
    if (strcmp(argv[0], "/usr/sbin/cfprefsd"))
    {
        return posix_spawnp_orig_wrapper(pid, path, action, attr, argv, envp);
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

    int ret = posix_spawnp_orig_wrapper(pid, path, action, attr, argv, newenvp);
    free(inj);
    free(newenvp);
    return ret;
}

int hook_posix_spawnp(pid_t *pid,
                      const char *path,
                      const posix_spawn_file_actions_t *action,
                      const posix_spawnattr_t *attr,
                      char *const argv[], char *envp[])
{
    /* pflags only work in launchd */
    if (getpid() == 1 && (pflags & palerain_option_rootful) == 0) {
        return hook_posix_spawnp_launchd(pid, path, action, attr, argv, envp);
    } else {
        char exe_path[PATH_MAX];
        uint32_t bufsize = PATH_MAX;
        int ret = _NSGetExecutablePath(exe_path, &bufsize);
        if (ret)
            abort();
        if (!strcmp("/usr/sbin/cfprefsd", path) && getppid() == 1 && !strcmp("/usr/libexec/xpcproxy", exe_path)) {
            return hook_posix_spawnp_xpcproxy(pid, path, action, attr, argv, envp);
        } else {
            return posix_spawnp_orig_wrapper(pid, path, action, attr, argv, envp);
        }
    }
}
DYLD_INTERPOSE(hook_posix_spawnp, posix_spawnp);
