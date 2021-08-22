#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const* argv[]) {
    int p1[2], p2[2];
    pipe(p1);
    pipe(p2);
    char byte;
    if (fork() == 0) {
        read(p1[0], &byte, 1);
        fprintf(1, "%d: received ping\n", getpid());
        write(p2[1], " ", 1);
    } else {
        write(p1[1], " ", 1);
        read(p2[0], &byte, 1);
        fprintf(1, "%d: received pong\n", getpid());
    }
    exit(0);
}
