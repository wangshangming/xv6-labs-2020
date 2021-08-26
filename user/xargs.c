#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXBUF 128

char *argvs[MAXARG];
int index = 0;
int xargs_argc;

void exec_(char *argvs[MAXARG]) {
    if (fork() == 0) {
        exec(argvs[0], argvs);
    }
    wait(0);
}

void process(char *buf) {
    index = xargs_argc - 1;
    char *argStart = buf, *argEnd = buf, *bufEnd = buf + strlen(buf);
    while (argStart < bufEnd) {
        argEnd = strchr(argStart, ' ');
        if (argEnd == 0) {
            argEnd = bufEnd;
        }

        if (index == MAXARG - 1) {
            fprintf(2, "the number(%d) of argumnets is exceed limit(%d)\n", index, MAXARG - 1);
            exit(1);
        }

        char arg[128];
        int len = argEnd - argStart;
        arg[len] = 0; // 这行代码不能跟下一行互换顺序。如果换了顺序，可能会导致编译时丢失了这行代码
        memcpy(arg, argStart, len);
        argvs[index++] = arg;

        argStart = argEnd + 1;
        argEnd = argStart;
    }

    argvs[index] = 0;
    exec_(argvs);
}

void readAndProcess() {
    char buf[MAXBUF], *p = buf;
    while (read(0, p, MAXBUF - 1 - (p - buf)) > 0 || p > buf) {
        char *lineEnd = strchr(buf, '\n');
        uint bufLen = strlen(buf);
        if (lineEnd == 0) {
            lineEnd = buf + bufLen;
        }
        *lineEnd = 0;

        process(buf);

        if (lineEnd < buf + bufLen) {
            bufLen -= lineEnd + 1 - buf;
            memmove(buf, lineEnd + 1, bufLen + 1);
            p = buf + bufLen;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc <= 1 || argc > MAXARG + 1) {
        fprintf(2, "Usage: xargs cmd param1 param2 ... param%d\n", MAXARG - 1);
        exit(1);
    }

    xargs_argc = argc;
    for (; index < argc - 1; index++) {
        argvs[index] = argv[index + 1];
    }

    readAndProcess();
    exit(0);
}
