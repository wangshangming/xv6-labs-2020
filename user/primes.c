#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int process(int parentPipe[2]) {
    pipe(parentPipe);
    int pid = fork();
    if (pid != 0) {
        return pid;
    }
    close(parentPipe[1]);

    int num, prime = -1;
    int childPipe[2];
    int childPid = -1;
    while (read(parentPipe[0], &num, sizeof(int)) > 0) {
        if (prime == -1) {
            prime = num;
            printf("prime %d\n", prime);
        } else if (num % prime != 0) {
            if (childPid == -1) {
                childPid = process(childPipe);
                close(childPipe[0]);
            }
            write(childPipe[1], &num, sizeof(int));
        }
    }
    if (childPid != -1) {
        close(childPipe[1]);
        wait(0);
    }    
    exit(0);
}

int main(int argc, char const* argv[]) {
    int p[2];
    process(p);
    close(p[0]);
    for (int num = 2; num <= 35; num++) {
        write(p[1], &num, sizeof(int));
    }
    close(p[1]);
    wait(0);
    exit(0);
}