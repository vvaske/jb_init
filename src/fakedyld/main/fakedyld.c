/*
 * jbinit 2.5
 * made by nick chan
*/

#include <fakedyld/fakedyld.h>
#include <mount_args.h>

int main() {
    int console_fd = open("/dev/console", O_RDWR, 0);
    int ret = 0;
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
    memory_file_handle_t payload;
    memory_file_handle_t payload15_dylib;
    if (pinfo.flags & palerain_option_bind_mount) {
        read_file("/payload15.dylib", &payload15_dylib);
        read_file("/payload", &payload);
    }
    mountroot(&pinfo, &sysinfo);
    prepare_rootfs(&sysinfo, &pinfo);
    init_cores(&sysinfo);
    if (sysinfo.osrelease.darwinMajor > 19) {
        write_file("/cores/payload.dylib", &payload15_dylib);
        write_file("/cores/payload", &payload);
    }
    get_and_patch_dyld();
    if (sysinfo.osrelease.darwinMajor > 19) {
        void* argv0 = mmap(NULL, sizeof("/sbin/launchd"), PROT_READ | PROT_WRITE, MAP_ANONYMOUS, 0, 0);
        void* envp0 = mmap(NULL, sizeof("DYLD_INSERT_LIBRARIES=/cores/payload.dylib"), PROT_READ | PROT_WRITE, MAP_ANONYMOUS, 0, 0);
        memcpy(argv0, "/sbin/launchd", sizeof("/sbin/launchd"));
        memcpy(envp0, "DYLD_INSERT_LIBRARIES=/cores/payload.dylib", sizeof("DYLD_INSERT_LIBRARIES=/cores/payload.dylib"));
        char** argv = mmap(NULL, (sizeof(char*)*2), PROT_READ | PROT_WRITE, MAP_ANONYMOUS, 0, 0);
        char** envp = mmap(NULL, (sizeof(char*)*2), PROT_READ | PROT_WRITE, MAP_ANONYMOUS, 0, 0);
        argv[0] = argv0;
        argv[1] = NULL;
        envp[0] = envp0;
        envp[1] = NULL;
        ret = execve(argv0, argv, envp);
    } else {
        void* argv0 = mmap(NULL, sizeof("/payload"), PROT_READ | PROT_WRITE, MAP_ANONYMOUS, 0, 0);
        char** argv = mmap(NULL, (sizeof(char*)*2), PROT_READ | PROT_WRITE, MAP_ANONYMOUS, 0, 0);
        memcpy(argv0, "/payload", sizeof("/payload"));
        argv[0] = argv0;
        argv[1] = NULL;
        ret = execve(argv0, argv, NULL);
    }
    LOG("execve failed with error=%d", ret);
    spin();
    __builtin_unreachable();
}

void spin() {
    LOG("jbinit DIED!");
    while(true) {
        sleep(5);
    }
}
