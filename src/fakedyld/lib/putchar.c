#include <fakedyld/fakedyld.h>

void _putchar(char character) {
    static size_t counter = 0;
    static char printbuf[0x100];
    printbuf[counter++] = character;
    if (character == '\n' || counter == sizeof(printbuf)) {
        write(STDOUT_FILENO, printbuf, counter);
        counter = 0;
    }
}
