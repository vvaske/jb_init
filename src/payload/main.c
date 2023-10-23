#include <stdio.h>
#include <payload/payload.h>
#include <libgen.h>
#include <string.h>

_Noreturn void spin() {
    while(1) sleep(86400);
}

int main(int argc, char* argv[]) {
    char* name = basename(argv[0]);
    if (!strcmp(name, "payload")) {
        return loader_main(argc, argv);
    } else if (!strcmp(name, "p1ctl")) {
        return p1ctl_main(argc, argv);
    } else {
        return -1;
    }
}
