#include "inlib.h"
#include "outlib.h"

#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAXLINE 256
#define MAXARGS 32

static void prompt(void) {
    out_puts("$ ");
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

static void run(char **argv) {
    if (!fork()) {
        execvp(argv[0], argv);
        out_puts("exec failed\n");
        _exit(1);
    }
    wait(0);
}

int main(void) {
    char line[MAXLINE];
    char *argv[MAXARGS];

    while (1) {
        prompt();

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
            if (argv[1])
                chdir(argv[1]);
            else
                chdir("/");
            continue;
        }

        run(argv);
    }
    return 0;
}
