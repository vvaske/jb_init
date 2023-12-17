#include <jbloader.h>

extern char** environ;

void display_bootscreen(void)
{
    run_async("/cores/binpack/bin/bootscreend", (char*[]){
        "/cores/binpack/bin/bootscreend",
        NULL});
    return;
}