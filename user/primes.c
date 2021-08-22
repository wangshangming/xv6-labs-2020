#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void write_pipe(int pipe, int num) {
    char buf[2] = {num / 10 + '0', num % 10 + '0'};
    write(pipe, buf, 2);
}

int read_pipe(int pipe, int *result) {
    char buf[2] = {'0', '0'};
    int n = read(pipe, buf, 2);
    *result = (buf[0] - '0') * 10 + (buf[1] - '0');
    return n;
}

void waitChild() {
    int status = 0;
    wait(&status);
}

int process(int p[2]) {
    pipe(p);
    int pid = fork();
    if (pid != 0) {
        return pid;
    }
    close(p[1]);
    int num, prime = -1;
    int nexPipe[2];
    int childPid = -1;
    while (read_pipe(p[0], &num) > 0) {
        if (prime == -1) {
            prime = num;
            printf("prime %d\n", prime);
            
        } else if (num % prime != 0) {
            if (childPid == -1) {
                childPid = process(nexPipe);
            }
            write_pipe(nexPipe[1], num);
        }
    }
    if (childPid != -1) {
        close(nexPipe[1]);
        waitChild();
    }    
    exit(0);
}

int main(int argc, char const* argv[]) {
    int p[2];
    process(p);
    for (char num = 2; num <= 35; num++) {
        write_pipe(p[1], num);
    }
    close(p[1]);
    waitChild();
    exit(0);
}