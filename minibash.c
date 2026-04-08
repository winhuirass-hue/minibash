#include "include/inlib.h"
#include "include/outlib.h"
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define MAXLINE 256
#define MAXARGS 32
#define MAXPATH 128

extern char **environ;

/* -------------------------------------------------
 * Parse command line into argv[]
 * ------------------------------------------------- */
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

/* -------------------------------------------------
 * Minimal execvp using execve + PATH
 * ------------------------------------------------- */
static void do_execve(char **argv) {
    /* Direct path */
    if (strchr(argv[0], '/')) {
        execve(argv[0], argv, environ);
        return;
    }

    /* Find PATH */
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

/* -------------------------------------------------
 * Main shell loop
 * ------------------------------------------------- */
int main(void) {
    char line[MAXLINE];
    char *argv[MAXARGS];

    while (1) {
        /* prompt */
        out_puts("$ ");

        int n = in_readline(line, sizeof(line));

        /* EOF (Ctrl+D) */
        if (n < 0)
            break;

        /* empty line */
        if (n == 0)
            continue;

        int argc = parse(line, argv);
        if (argc == 0)
            continue;

        /* builtins */
        if (!strcmp(argv[0], "exit"))
            break;

        if (!strcmp(argv[0], "cd")) {
            if (argv[1])
                chdir(argv[1]);
            else
                chdir("/");
            continue;
        }

        /* fork + exec */
        pid_t pid = fork();
        if (pid == 0) {
            do_execve(argv);

            /* exec failed */
            out_puts("minibash: cannot execute ");
            out_puts(argv[0]);
            out_puts(" (errno=");
            out_putu(errno);
            out_puts(")\n");

            _exit(127);
        }

        int st;
        wait(&st);

        /* show exit info */
        if (WIFEXITED(st)) {
            int code = WEXITSTATUS(st);
            if (code != 0) {
                out_puts("exit code: ");
                out_putu(code);
                out_putc('\n');
            }
        } else if (WIFSIGNALED(st)) {
            out_puts("terminated by signal ");
            out_putu(WTERMSIG(st));
            out_putc('\n');
        }
    }

    return 0;
}
