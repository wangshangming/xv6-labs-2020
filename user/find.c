#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9.

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{  // must look at empty string
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}


void find(char* path, char* pattern) {
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
        if (match(pattern, p) == 1) {
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
        find(buf, pattern);
    }
    close(fd);
}

int main(int argc, char* argv[]) {
    char *path, *pattern;
    if (argc < 2) {
        fprintf(2, "Usage: find ${path} ${pattern}\n or find ${pattern}……");
        exit(1);
    }

    if (argc == 2) {
        path = ".";
        pattern = argv[1];
    } else {
        path = argv[1];
        pattern = argv[2];
    }

    find(path, pattern);
    exit(0);
}
