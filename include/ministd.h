#ifndef MINISTD_H
#define MINISTD_H

/* базові типи (без stdint) */
typedef unsigned long size_t;

/* ================= string ================= */

static inline size_t mstrlen(const char *s) {
    size_t n = 0;
    while (s && s[n]) n++;
    return n;
}

static inline int mstreq(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == 0 && *b == 0;
}

static inline int mstrncmp(const char *a, const char *b, size_t n) {
    while (n && *a && *b) {
        if (*a != *b) return (int)(*(unsigned char*)a - *(unsigned char*)b);
        a++; b++; n--;
    }
    if (!n) return 0;
    return (int)(*(unsigned char*)a - *(unsigned char*)b);
}

static inline char *mstrchr(const char *s, char c) {
    while (*s) {
        if (*s == c) return (char*)s;
        s++;
    }
    return 0;
}

static inline char *mstrrchr(const char *s, char c) {
    const char *p = 0;
    while (*s) {
        if (*s == c) p = s;
        s++;
    }
    return (char*)p;
}

/* ================= memory ================= */

static inline void *mmemcpy(void *d, const void *s, size_t n) {
    char *dd = (char*)d;
    const char *ss = (const char*)s;
    while (n--) *dd++ = *ss++;
    return d;
}

static inline void *mmemset(void *d, int v, size_t n) {
    unsigned char *p = (unsigned char*)d;
    while (n--) *p++ = (unsigned char)v;
    return d;
}

/* ================= numbers ================= */

static inline int matoi(const char *s) {
    int r = 0, neg = 0;
    if (!s) return 0;
    if (*s == '-') { neg = 1; s++; }
    while (*s >= '0' && *s <= '9') {
        r = r * 10 + (*s - '0');
        s++;
    }
    return neg ? -r : r;
}

#endif /* MINISTD_H */
