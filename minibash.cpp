#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

int run(char **argv, int in, int out) {
    if (!fork()) {
        if (in  != 0)  dup2(in,  0);
        if (out != 1)  dup2(out, 1);
        execvp(argv[0], argv);
        _exit(127);
    }
    return 0;
}

int main() {
    char buf[256], *argv[32], *cmds[8][16];
    int pipes[2], n;

    while (1) {
        write(1, "$ ", 2);
        if (!fgets(buf, sizeof(buf), stdin)) break;
        buf[strcspn(buf, "\n")] = 0;
        if (!buf[0]) continue;
        if (!strcmp(buf, "exit")) break;

        /* split by | */
        n = 0;
        cmds[n][0] = strtok(buf, " ");
        int i = 1;
        while ((cmds[n][i] = strtok(0, " "))) {
            if (!strcmp(cmds[n][i], "|")) {
                cmds[n][i] = 0;
                cmds[++n][0] = strtok(0, " ");
                i = 1;
            } else i++;
        }
        n++;

        /* single builtin cd */
        if (n == 1 && !strcmp(cmds[0][0], "cd")) {
            chdir(cmds[0][1] ? cmds[0][1] : "/");
            continue;
        }

        int in = 0;
        for (i = 0; i < n; i++) {
            pipe(pipes);
            run(cmds[i], in, i < n - 1 ? pipes[1] : 1);
            close(pipes[1]);
            if (in) close(in);
            in = pipes[0];
        }
        while (wait(0) > 0);
    }
}
