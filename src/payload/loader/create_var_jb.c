#include <payload/payload.h>
#include <removefile.h>
#include <sys/mount.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/utsname.h>
#include <CoreFoundation/CoreFoundation.h>
#include <sys/stat.h>

int get_prebootPath(char jbPath[150]) {
    struct utsname name;
    uname(&name);
    if (atoi(name.release) < 20) return ENOTSUP;
    char bmhash[97];
    get_bmhash(bmhash);
    snprintf(jbPath, 150, "/private/preboot/%s", bmhash);
    DIR* dir = opendir(jbPath);
    if (!dir) {
        return ENOENT;
    }
    char jbPathName[20];
    bool has_prebootjb = false;
    struct dirent* d;
    while ((d = readdir(dir)) != NULL) {
        if (strncmp(d->d_name, "jb-", 3)) continue;
        if (has_prebootjb == true) {
            closedir(dir);
            return EEXIST;
        }
        snprintf(jbPathName, 20, "%s" ,d->d_name);
        has_prebootjb = true;
    }
    closedir(dir);
    if (!has_prebootjb) return ENOENT;
    snprintf(jbPath, 150, "/private/preboot/%s/%s/procursus", bmhash, jbPathName);
    struct stat st;
    if ((stat(jbPath, &st))) {
        return ENOENT;
    }
    if (!S_ISDIR(st.st_mode)) {
        return ENOTDIR;
    }
    return KERN_SUCCESS;
}

int create_var_jb() {
    if (access("/var/jb", F_OK) == 0) return 0;
    char prebootPath[150];
    int ret = get_prebootPath(prebootPath);
    switch (ret) {
        case ENOTSUP:
        case ENOENT:
            return 0;
        case EEXIST:
            fprintf(stderr, "duplicate preboot jailbreak path\n");
            return -1;
        case ENOTDIR:
            fprintf(stderr, "jailbreak path is not a directory\n");
            return -1;
        case KERN_SUCCESS:
            printf("/var/jb -> %s\n", prebootPath);
            break;
        default:
            fprintf(stderr, "cannot create /var/jb\n");
            return -1;
    }
    return symlink(prebootPath, "/var/jb");
}
