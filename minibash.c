#include "include/inlib.h"
#include "include/outlib.h"
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAXLINE 256
#define MAXARGS 32
#define MAXPATH 128

extern char **environ;

/* -------------------------------------------------
 * Try execve with PATH lookup (minimal execvp clone)
 * ------------------------------------------------- */
static void do_execve(char **argv) {
    /* If command contains '/', try directly */
    if (strchr(argv[0], '/')) {
        execve(argv[0], argv, environ);
        return;
    }

    /* Get PATH */
    char *path = 0;
    for (char **e = environ; *e; e++) {
        if (!strncmp(*e, "PATH=", 5)) {
            path = *e + 5;
            break;
        }
    }

    if (!path)
        return;

    char full[MAXPATH];
    while (*path) {
        char *p = full;

        /* copy directory */
        while (*path && *path != ':')
            *p++ = *path++;

        if (*path == ':')
            path++;

        *p++ = '/';

        /* copy command */
        char *c = argv[0];
        while (*c)
            *p++ = *c++;

        *p = 0;
        execve(full, argv, environ);
    }
}

static int parse(char *line, char **argv) {
    int n = 0;

    while (*line && n < MAXARGS - 1) {
        while (*line == ' ')
            *line++ = 0;
        if (!*line)
            break;
        argv[n++] = line;
        while (*line && *line != ' ')
            line++;
    }
    argv[n] = 0;
    return n;
}

int main(void) {
    char line[MAXLINE];
    char *argv[MAXARGS];

    while (1) {
        out_puts("$ ");

        if (in_readline(line, sizeof(line)) <= 0)
            break;

        if (!line[0])
            continue;

        int argc = parse(line, argv);
        if (argc == 0)
            continue;

        if (!strcmp(argv[0], "exit"))
            break;

        if (!strcmp(argv[0], "cd")) {
            chdir(argv[1] ? argv[1] : "/");
            continue;
        }

        if (!fork()) {
            do_execve(argv);
            out_puts("command not found\n");
            _exit(1);
        }
        wait(0);
    }
    return 0;
}
