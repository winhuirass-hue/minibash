#include "include/inlib.h"
#include "include/outlib.h"
#include "include/promt.h"
#include "include/ministd.h"
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define MAXLINE 256
#define MAXARGS 32
#define MAXCMDS 8
#define MAXPATH 128

extern char **environ;

/* -------- parse words -------- */
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

/* -------- split by | -------- */
static int parse_pipeline(char *line, char *cmds[]) {
    int n = 0;
    cmds[n++] = line;
    while (*line) {
        if (*line == '|') {
            *line = 0;
            if (n < MAXCMDS) cmds[n++] = line + 1;
        }
        line++;
    }
    return n;
}

/* -------- execve + PATH ------- */
static void do_execve(char **argv) {
    if (strchr(argv[0], '/')) {
        execve(argv[0], argv, environ);
        return;
    }
    char *path = 0;
    for (char **e = environ; *e; e++) {
        if (!strncmp(*e, "PATH=", 5)) { path = *e + 5; break; }
    }
    if (!path) return;

    char full[MAXPATH];
    while (*path) {
        char *p = full;
        while (*path && *path != ':') *p++ = *path++;
        if (*path == ':') path++;
        *p++ = '/';
        char *c = argv[0];
        while (*c) *p++ = *c++;
        *p = 0;
        execve(full, argv, environ);
    }
}

/* -------- run pipeline -------- */
/* returns exit code of the LAST command */
static int run_pipeline(char *cmds[], int ncmd, int background) {
    int prev_fd = -1, pipes[2];
    pid_t pids[MAXCMDS];
    int last_status = 0;

    for (int i = 0; i < ncmd; i++) {
        char *argv[MAXARGS];
        parse_words(cmds[i], argv);
        if (!argv[0]) return 0;

        if (i < ncmd - 1) pipe(pipes);

        pid_t pid = fork();
        if (pid == 0) {
            signal(SIGINT, SIG_DFL);
            if (prev_fd != -1) { dup2(prev_fd, 0); close(prev_fd); }
            if (i < ncmd - 1) {
                close(pipes[0]);
                dup2(pipes[1], 1);
                close(pipes[1]);
            }
            do_execve(argv);
            out_puts("minibash: cannot execute ");
            out_puts(argv[0]);
            out_puts(" (errno=");
            out_putu(errno);
            out_puts(")\n");
            _exit(127);
        }

        pids[i] = pid;

        if (prev_fd != -1) close(prev_fd);
        if (i < ncmd - 1) {
            close(pipes[1]);
            prev_fd = pipes[0];
        }
    }

    if (background) {
        /* do not wait */
        return 0;
    }

    for (int i = 0; i < ncmd; i++) {
        int st;
        waitpid(pids[i], &st, 0);
        if (i == ncmd - 1) {
            if (WIFEXITED(st)) last_status = WEXITSTATUS(st);
            else if (WIFSIGNALED(st)) last_status = 128 + WTERMSIG(st);
        }
    }
    return last_status;
}

/* -------- trim helpers -------- */
static char *trim(char *s) {
    while (*s == ' ') s++;
    char *e = s + strlen(s);
    while (e > s && e[-1] == ' ') *--e = 0;
    return s;
}

/* -------- main --------------- */
int main(void) {
    char line[MAXLINE];
    char *cmds[MAXCMDS];

    signal(SIGINT, SIG_IGN); /* shell ignores Ctrl+C */

    while (1) {
        prompt();

        int n = in_readline(line, sizeof(line));
        if (n < 0) break;       /* EOF */
        if (n == 0) continue;  /* empty */

        char *s = trim(line);

        if (!strncmp(s, "cd", 2)) {
            char *p = s + 2; while (*p == ' ') p++;
            chdir(*p ? p : "/");
         if (!strncmp(s, "exec", 4)) {
            char *p = s + 4;
            while (*p == ' ') p++;

        if (!*p) {
             out_puts("minibash: exec: missing command\n");
             continue;
    }

             char *argv[MAXARGS];
             parse_words(p, argv);

             do_execve(argv);

    /* if we are here — exec failed */
             out_puts("minibash: exec failed ");
             out_puts(argv[0]);
             out_puts(" (errno=");
             out_putu(errno);
             out_puts(")\n");

           _exit(127);
    }   
            continue;
        }

        /* ---- background '&' at end ---- */
        int background = 0;
        int len = strlen(s);
        if (len && s[len - 1] == '&') {
            background = 1;
            s[len - 1] = 0;
            s = trim(s);
        }

        /* ---- logical AND '&&' ---- */
        char *andp = strstr(s, "&&");
        if (andp) {
            *andp = 0;
            char *left = trim(s);
            char *right = trim(andp + 2);

            int ncmd = parse_pipeline(left, cmds);
            int st = run_pipeline(cmds, ncmd, background);
            if (st == 0 && right[0]) {
                ncmd = parse_pipeline(right, cmds);
                run_pipeline(cmds, ncmd, background);
            }
            continue;
        }

        /* ---- normal / pipeline ---- */
        int ncmd = parse_pipeline(s, cmds);
        run_pipeline(cmds, ncmd, background);
    }
    return 0;
}
