#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char* path, char* name) {
    int fd;
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    if (st.type == T_FILE) {
        char* p = path + strlen(path) - 1;
        while (*p != '/') {
            p--;
        }
        p++;
        if (strcmp(p, name) == 0) {
            printf("%s\n", path);
        }
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        close(fd);
        return;
    }

    char buf[512], *p;
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    struct dirent de;
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0 || strcmp(de.name, ".") == 0 ||
            strcmp(de.name, "..") == 0) {
            continue;
        }
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        find(buf, name);
    }
    close(fd);
}

int main(int argc, char* argv[]) {
    char *path, *name;
    if (argc < 2) {
        fprintf(2, "Usage: find ${path} ${name}\n or find ${name}……");
        exit(1);
    }

    if (argc == 2) {
        path = ".";
        name = argv[1];
    } else {
        path = argv[1];
        name = argv[2];
    }

    find(path, name);
    exit(0);
}
