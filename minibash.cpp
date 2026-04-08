#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

int main() {
    char buf[256];
    char* argv[32];

    while (1) {
        write(1, "$ ", 2);
        if (!fgets(buf, sizeof(buf), stdin)) break;

        buf[strcspn(buf, "\n")] = 0;
        if (!buf[0]) continue;
        if (!strcmp(buf, "exit")) break;

        int i = 0;
        argv[i++] = strtok(buf, " ");
        while ((argv[i++] = strtok(0, " ")));

        if (!strcmp(argv[0], "cd")) {
            chdir(argv[1] ? argv[1] : "/");
            continue;
        }

        if (!fork()) {
            execvp(argv[0], argv);
            _exit(1);
        }
        wait(0);
    }
}
