/*
 * jbinit 2.5
 * made by nick chan
*/

#include <fakedyld/fakedyld.h>
#include <mount_args.h>

int main() {
    int console_fd = open("/dev/console", O_RDWR | O_CLOEXEC | O_SYNC, 0);
    set_fd_console(console_fd);
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
        read_file("/payload.dylib", &payload15_dylib);
        read_file("/payload", &payload);
    }
    mountroot(&pinfo, &sysinfo);
    prepare_rootfs(&sysinfo, &pinfo);
    memory_file_handle_t dyld_handle;
    read_file("/usr/lib/dyld", &dyld_handle);
    check_dyld(&dyld_handle);
    int platform = get_platform(&dyld_handle);
    init_cores(&sysinfo, platform);
    patch_dyld(&dyld_handle, platform);
    write_file("/cores/dyld", &dyld_handle);
    if (sysinfo.osrelease.darwinMajor > 20) {
        write_file("/cores/payload.dylib", &payload15_dylib);
        write_file("/cores/payload", &payload);
    } else {
        symlink("/payload", "/cores/payload");
        symlink("/payload.dylib", "/cores/payload.dylib");
    }
    // set_fd_console(1);
    // close(console_fd);
    /*
        argv0/execve_buffer -> =============================
                                      "/sbin/launchd"
                     envp0 ->  =============================
                                "DYLD_INSERT_LIBRARIES..."
            argv, &argv[0] ->  =============================
                                        argv0 (ptr)
                   &argv[1] -> =============================
                                        nullptr
             envp, &envp[0] -> =============================
                                        envp0 (ptr)
                  &envp[1] ->  =============================
                                         nullptr
                               =============================

    */
    /*if (sysinfo.osrelease.darwinMajor > 19) {*/
        char* execve_buffer = mmap(NULL, 0x4000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
        if (execve_buffer == NULL) {
            printf("mmap for execve failed\n");
            spin();
        }
        char* argv0 = execve_buffer;
        char* envp0 = (execve_buffer + sizeof("/sbin/launchd"));
        memcpy(argv0, "/sbin/launchd", sizeof("/sbin/launchd"));
        memcpy(envp0, "DYLD_INSERT_LIBRARIES=/cores/payload.dylib", sizeof("DYLD_INSERT_LIBRARIES=/cores/payload.dylib"));
        char** argv = (char**)((char*)envp0 + sizeof("DYLD_INSERT_LIBRARIES=/cores/payload.dylib"));
        char** envp = (char**)((char*)argv + (2*sizeof(char*)));
        argv[0] = argv0;
        argv[1] = NULL;
        envp[0] = envp0;
        envp[1] = NULL;
        LOG("argv0: %s, envp0: %s, argv[0]: %s, envp[0]: %s", argv0, envp0, argv[0], envp[0]);
        ret = execve(argv0, argv, envp);
    /*}*/
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
