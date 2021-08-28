#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const* argv[]) {
    int parent2Child[2], child2Parent[2];
    pipe(parent2Child);
    pipe(child2Parent);
    char byte;
    if (fork() == 0) {
        close(parent2Child[1]);
        close(child2Parent[0]);
        
        read(parent2Child[0], &byte, 1);
        printf("%d: received ping\n", getpid());
        write(child2Parent[1], " ", 1);

        close(parent2Child[0]);
        close(child2Parent[1]);
    } else {
        close(parent2Child[0]);
        close(child2Parent[1]);

        write(parent2Child[1], " ", 1);
        read(child2Parent[0], &byte, 1);
        printf("%d: received pong\n", getpid());

        close(parent2Child[1]);
        close(child2Parent[0]);
    }
    exit(0);
}
