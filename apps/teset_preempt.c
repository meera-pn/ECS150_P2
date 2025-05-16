#include <stdio.h>
#include "uthread.h"

#define MAX_PRINTS 10

void spin(void *arg) {
    char *name = (char *)arg;
    for (int i = 0; i < MAX_PRINTS; i++)
        printf("%s\n", name);
}

int main() {
    uthread_create(spin, "A");
    uthread_run(true, spin, "B");
    return 0;
}
