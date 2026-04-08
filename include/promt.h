#ifndef PROMT_H
#define PROMT_H

#include "./outlib.h"

#include <unistd.h>
#include <sys/types.h>


#ifdef __GLIBC__
#include <pwd.h>
#endif


static void prompt(void) {
    char cwd[128];
    const char *user = "user";

    /* username */
    struct passwd *pw = getpwuid(getuid());
    if (pw && pw->pw_name)
        user = pw->pw_name;

    /* current directory */
    if (!getcwd(cwd, sizeof(cwd)))
        cwd[0] = '?', cwd[1] = 0;

    /* print: user@cwd */
    out_puts(user);
    out_putc('@');
    out_puts(cwd);

    /* root or user */
    if (geteuid() == 0)
        out_puts("# ");
    else
        out_puts("$ ");
}

#endif /* PROMPT_H */
