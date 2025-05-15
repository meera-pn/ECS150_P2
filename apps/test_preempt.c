#include <stdio.h>
#include <stdlib.h>
#include "uthread.h"

void loop_printer(void *arg) {
    char *label = (char *)arg;
    int count = 0;
    while (count++ < 10) {
        for (int i = 0; i < 1000000; i++);
        printf("%s\n", label);
        uthread_yield();
    }
    uthread_exit();
}

int main() {
    uthread_create(loop_printer, "A");
    uthread_create(loop_printer, "B");
    uthread_run(true, loop_printer, "Main");  
    return 0;
}