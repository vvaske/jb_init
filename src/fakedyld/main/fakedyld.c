#include <fakedyld/fakedyld.h>

int main(int argc, char* argv[], char* envp[], char* apple[]) {
    int console_fd = open("/dev/console", O_RDWR, 0);
    set_console_fd(console_fd);
    if (console_fd == -1) spin();
    printf(
        "=============================================================\n"
        "!!! NICK CHAN AND PLOOSH DYNAMICALLY LINKING AND PATCHING !!!\n"
        "=============================================================\n"
        "                   (c) 2023 palera1n team                    \n"
        "=============================================================\n"
    );
    LOG("argc: %d", argc);
    printf("fakedyld: Invoked as: ");
    for (uint32_t i = 0; argv[i] != NULL; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");
    printf("fakedyld: environment: ");
    for (uint32_t i = 0; envp[i] != NULL; i++) {
        printf("%s\n", envp[i]);
    }
    printf("\n");
    struct paleinfo pinfo;
    get_pinfo(&pinfo);
    printf(
        "kbase: 0x%llx\n"
        "kslide: 0x%llx\n"
        "flags: 0x%llx\n"
        "rootdev: %s\n"
    ,pinfo.kbase,pinfo.kslide,pinfo.flags,pinfo.rootdev
    );
       pinfo_check(&pinfo);
    struct systeminfo sysinfo;
    systeminfo(&sysinfo);
    LOG("Parsed kernel version: Darwin %d.%d.%d xnu: %d",
        sysinfo.osrelease.darwinMajor,
        sysinfo.osrelease.darwinMinor,
        sysinfo.osrelease.darwinRevision,
        sysinfo.xnuMajor
    );
    LOG("boot-args: %s", sysinfo.bootargs);
    LOG("Kernel version (raw): %s", sysinfo.kversion);
    if (strstr(sysinfo.bootargs, "wdt-1") == NULL && (pinfo.flags & palerain_option_setup_rootful)) {
        LOG("setup rootful requires wdt=-1 in boot-args");
        spin();
    }
    sleep(1);
    for (int i = 0; i <= ELAST; i++) {
        LOG("Test error: %d (%s)", i, strerror(i));
        sleep(1);
    }
    spin();
    CHECK_ERROR(mount("hfs", "/", MNT_RDONLY | MNT_UPDATE, RAMDISK), "remount ramdisk failed");
    rootwait(&sysinfo);
    mountroot(&pinfo, &sysinfo);

    spin();
    __builtin_unreachable();
}
