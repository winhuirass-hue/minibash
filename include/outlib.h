#ifndef OUTLIB_H
#define OUTLIB_H

#include <unistd.h>   /* write */

/* -------------------------------------------------
 * Write raw buffer to given file descriptor
 * Returns bytes written
 * ------------------------------------------------- */
static inline int out_write(int fd, const void *buf, unsigned long len) {
    return write(fd, buf, len);
}

/* -------------------------------------------------
 * Write one character to stdout
 * ------------------------------------------------- */
static inline void out_putc(char c) {
    write(1, &c, 1);
}

/* -------------------------------------------------
 * Write C-string to stdout
 * ------------------------------------------------- */
static inline void out_puts(const char *s) {
    const char *p = s;
    while (*p) p++;
    write(1, s, p - s);
}

/* -------------------------------------------------
 * Write unsigned integer (decimal)
 * ------------------------------------------------- */
static inline void out_putu(unsigned int v) {
    char buf[10];
    int i = 0;

    if (v == 0) {
        out_putc('0');
        return;
    }

    while (v) {
        buf[i++] = '0' + (v % 10);
        v /= 10;
    }
    while (i--)
        out_putc(buf[i]);
}

/* -------------------------------------------------
 * Write signed integer (decimal)
 * ------------------------------------------------- */
static inline void out_puti(int v) {
    if (v < 0) {
        out_putc('-');
        out_putu((unsigned int)(-v));
    } else {
        out_putu((unsigned int)v);
    }
}

/* -------------------------------------------------
 * Write hexadecimal integer (lowercase)
 * ------------------------------------------------- */
static inline void out_puthex(unsigned int v) {
    static const char hex[] = "0123456789abcdef";
    char buf[8];
    int i = 0;

    if (v == 0) {
        out_putc('0');
        return;
    }

    while (v) {
        buf[i++] = hex[v & 0xF];
        v >>= 4;
    }
    while (i--)
        out_putc(buf[i]);
}

/* -------------------------------------------------
 * Write pointer (0xXXXXXXXX)
 * ------------------------------------------------- */
static inline void out_putptr(const void *p) {
    out_puts("0x");
    out_puthex((unsigned long)p);
}

#endif /* OUTLIB_H */
