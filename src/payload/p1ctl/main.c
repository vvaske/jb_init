#include <paleinfo.h>
#include <payload/payload.h>
#include <stdio.h>

/*
 * TODO: Actually implement p1ctl instead of just printing flags
 * which is required for boot
*/
int p1ctl_main(int argc, char* argv[]) {
    struct paleinfo pinfo;
    int ret = get_pinfo(&pinfo);
    if (ret == -1) return -1;
    printf("0x%016llX\n", pinfo.flags);
    return 0;
}
