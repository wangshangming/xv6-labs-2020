#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc <= 1 || argc > MAXARG + 1) {
        fprintf(2, "Usage: xargs cmd param1 param2 ... param%d\n", MAXARG - 1);
        exit(1);
    }

    char *argvs[MAXARG];
    for (int i = 0; i < argc - 1; i++) {
        argvs[i] = argv[i + 1];
    }

    char buf[128], *p = buf;
    while (read(0, p, 127 - (p - buf)) > 0 || p > buf) {
        int i = argc - 1;
        char *lineEnd = strchr(buf, '\n');
        if (lineEnd == 0) {
            lineEnd = buf + strlen(buf);
        }
        char *argStart = buf, *argEnd = buf;
        while (argStart < lineEnd) {
            argEnd = strchr(argStart, ' ');
            if (argEnd == 0) {
                argEnd = lineEnd;
            }

            if (i == MAXARG - 1) {
                fprintf(2, "the number(%d) of argumnets is exceed limit(%d)\n",
                        i - 1, MAXARG);
                exit(1);
            }

            char arg[128];
            int len = argEnd - argStart;
            arg[len] = 0;
            memcpy(arg, argStart, len);
            argvs[i++] = arg;

            argStart = argEnd + 1;
            argEnd = argStart;
        }
        argvs[i++] = 0;
        if (fork() == 0) {
            exec(argvs[0], argvs);
        }
        wait(0);

        memmove(buf, lineEnd + 1, strlen(buf) - (lineEnd + 1 - buf) + 1);
        p = buf + strlen(buf);
    }
    exit(0);
}
