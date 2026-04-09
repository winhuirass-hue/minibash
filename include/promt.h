#ifndef PROMT_H
#define PROMT_H

#include "./outlib.h"

#include <unistd.h>
#include <sys/types.h>


static void prompt(void) {
    char cwd[128];
    const char *user = "user";


    /* current directory */
    if (!getcwd(cwd, sizeof(cwd)))
        cwd[0] = '?', cwd[1] = 0;

    /* print: user@cwd */
    out_puts(cwd);

    /* root or user */
    if (geteuid() == 0)
        out_puts("# ");
    else
        out_puts("$ ");
}

#endif /* PROMPT_H */
