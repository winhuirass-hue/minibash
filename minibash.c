#include "include/inlib.h"
#include "include/outlib.h"
#include "include/promt.h"
#include "include/ministd.h"
#include "include/minicalls.h"

#define MAXLINE 256
#define MAXARGS 32
#define MAXCMDS 8
#define MAXPATH 128

extern char **environ;

/* ------------ parse words ------------ */
static int parse_words(char *line, char **argv) {
    int n = 0;
    while (*line && n < MAXARGS - 1) {
        while (*line == ' ') *line++ = 0;
        if (!*line) break;
        argv[n++] = line;
        while (*line && *line != ' ') line++;
    }
    argv[n] = 0;
    return n;
}

/* ------------ split pipeline ---------- */
static int parse_pipeline(char *line, char **cmds) {
    int n = 0;
    cmds[n++] = line;
    while (*line) {
        if (*line == '|') {
            *line = 0;
            if (n < MAXCMDS)
                cmds[n++] = line + 1;
        }
        line++;
    }
    return n;
}

/* ------------ exec with PATH ---------- */
static void do_execve(char **argv) {
    char *path;
    char full[MAXPATH];

    if (mstrchr(argv[0], '/')) {
        execve(argv[0], argv, environ);
        return;
    }

    path = 0;
    for (char **e = environ; *e; e++) {
        if (mstrncmp(*e, "PATH=", 5) == 0) {
            path = *e + 5;
            break;
        }
    }
    if (!path) return;

    while (*path) {
        char *p = full;
        while (*path && *path != ':')
            *p++ = *path++;
        if (*path == ':') path++;

        *p++ = '/';
        char *c = argv[0];
        while (*c) *p++ = *c++;
        *p = 0;

        execve(full, argv, environ);
    }
}

/* ------------ run pipeline ------------ */
static void run_pipeline(char **cmds, int ncmd) {
    int prev = -1;
    int fd[2];

    for (int i = 0; i < ncmd; i++) {
        char *argv[MAXARGS];
        parse_words(cmds[i], argv);
        if (!argv[0]) _exit(0);

        if (i < ncmd - 1)
            pipe(fd);

        int pid = fork();
        if (pid == 0) {
            if (prev != -1) {
                dup2(prev, 0);
                close(prev);
            }
            if (i < ncmd - 1) {
                close(fd[0]);
                dup2(fd[1], 1);
                close(fd[1]);
            }
            do_execve(argv);
            out_puts("minibash: cannot execute ");
            out_puts(argv[0]);
            out_putc('\n');
            _exit(127);
        }

        if (prev != -1) close(prev);
        if (i < ncmd - 1) {
            close(fd[1]);
            prev = fd[0];
        }
    }

    /* wait for all */
    while (waitpid(-1, 0, 0) > 0);
}

/* ------------ trim helpers ------------ */
static char *trim(char *s) {
    char *e;
    while (*s == ' ') s++;
    e = s + mstrlen(s);
    while (e > s && e[-1] == ' ') *--e = 0;
    return s;
}

/* ------------ main -------------------- */
int main(void) {
    char line[MAXLINE];
    char *cmds[MAXCMDS];

    while (1) {
        prompt();

        int n = in_readline(line, sizeof(line));
        if (n < 0) _exit(0);
        if (n == 0) continue;

        char *s = trim(line);

        /* builtin exit */
        if (mstreq(s, "exit")) {
            _exit(0);
        }

        /* builtin cd */
        if (mstrncmp(s, "cd", 2) == 0) {
            char *p = s + 2;
            while (*p == ' ') p++;
            chdir(*p ? p : "/");
            continue;
        }

        /* builtin exec */
        if (mstrncmp(s, "exec", 4) == 0) {
            char *p = s + 4;
            while (*p == ' ') p++;
            if (!*p) {
                out_puts("minibash: exec: missing command\n");
                continue;
            }
            char *argv[MAXARGS];
            parse_words(p, argv);
            do_execve(argv);
            out_puts("minibash: exec failed\n");
            _exit(127);
        }

        int ncmd = parse_pipeline(s, cmds);
        run_pipeline(cmds, ncmd);
    }
}
