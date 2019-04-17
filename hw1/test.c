#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    pid_t pid = getpid();
    pid_t pgrp = getpgrp();
    printf("Parent: %d %d\n", pid, pgrp);
    pid_t cpid = fork();
    if (cpid > 0) {
        int status;
        wait(&status);
        char c = getchar();
    }
    else {
        pid_t cpgrp = getpgrp();
        pid_t ccpid = getpid();
        printf("Child: %d %d\n", ccpid, cpgrp);
    }
    return 0;
}
