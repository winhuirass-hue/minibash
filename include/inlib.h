#ifndef INLIB_H
#define INLIB_H

long read(int, void*, unsigned long);


/* -------------------------------------------------
 * Read one character from stdin
 * Returns character or -1 on EOF/error
 * ------------------------------------------------- */
static inline int in_getc(void) {
    char c;
    if (read(0, &c, 1) != 1)
        return -1;
    return (unsigned char)c;
}

/* -------------------------------------------------
 * Read a line into buffer
 * Newline is replaced with '\0'
 * Returns number of bytes read
 * ------------------------------------------------- */
static inline int in_readline(char *buf, int max) {
    int i = 0;
    char c;
    int r;

    while (i < max - 1) {
        r = read(0, &c, 1);
        if (r == 0)      /* EOF */
            return -1;
        if (r < 0)
            return -1;
        if (c == '\n')
            break;
        buf[i++] = c;
    }
    buf[i] = 0;
    return i;
}

/* -------------------------------------------------
 * Read unsigned decimal integer
 * Returns value or -1 on error
 * ------------------------------------------------- */
static inline int in_readu(void) {
    int v = 0;
    int c;

    while ((c = in_getc()) == ' ' || c == '\n');
    if (c < '0' || c > '9')
        return -1;

    do {
        v = v * 10 + (c - '0');
        c = in_getc();
    } while (c >= '0' && c <= '9');

    return v;
}

/* -------------------------------------------------
 * Read signed decimal integer
 * ------------------------------------------------- */
static inline int in_readi(void) {
    int sign = 1;
    int c, v;

    while ((c = in_getc()) == ' ' || c == '\n');
    if (c == '-') {
        sign = -1;
        c = in_getc();
    }
    if (c < '0' || c > '9')
        return -1;

    v = 0;
    do {
        v = v * 10 + (c - '0');
        c = in_getc();
    } while (c >= '0' && c <= '9');

    return sign * v;
}

/* -------------------------------------------------
 * Read hexadecimal integer
 * Supports optional 0x / 0X prefix
 * ------------------------------------------------- */
static inline int in_readhex(void) {
    int v = 0;
    int c;

    while ((c = in_getc()) == ' ' || c == '\n');

    if (c == '0') {
        int n = in_getc();
        if (n == 'x' || n == 'X')
            c = in_getc();
        else
            c = n;
    }

    if (!((c >= '0' && c <= '9') ||
          (c >= 'a' && c <= 'f') ||
          (c >= 'A' && c <= 'F')))
        return -1;

    do {
        v <<= 4;
        if (c >= '0' && c <= '9')
            v |= c - '0';
        else if (c >= 'a' && c <= 'f')
            v |= c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            v |= c - 'A' + 10;
        else
            break;
        c = in_getc();
    } while (1);

    return v;
}

#endif /* INLIB_H */
