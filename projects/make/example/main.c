#include "main.h"
#include <ev3.h>

int main(int argc, char **argv) {
    for (int i = 0; i < REPEAT; i++) {
        TermPrintf("[%d] Hello, World!\r\n", i + 1);
        Wait(1000);
    }
    return 0;
}
