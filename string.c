#include "string.h"

int strlen(const char *s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, int n) {
    for (int i = 0; i < n; i++) {
        if (s1[i] != s2[i]) return s1[i] - s2[i];
        if (s1[i] == '\0') return 0;
    }
    return 0;
}

void strcpy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void strcat(char *dest, const char *src) {
    while (*dest) dest++;
    while (*src) *dest++ = *src++;
    *dest = '\0';
}

void *memset(void *ptr, int value, int size) {
    unsigned char *p = (unsigned char *)ptr;
    for (int i = 0; i < size; i++) {
        p[i] = (unsigned char)value;
    }
    return ptr;
}

void *memcpy(void *dest, const void *src, int size) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    for (int i = 0; i < size; i++) {
        d[i] = s[i];
    }
    return dest;
}

void int_to_str(int num, char *buf) {
    if (num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    int negative = 0;
    if (num < 0) {
        negative = 1;
        num = -num;
    }

    // Build string in reverse
    char tmp[12];
    int i = 0;
    while (num > 0) {
        tmp[i++] = '0' + (num % 10);
        num /= 10;
    }

    int j = 0;
    if (negative) buf[j++] = '-';

    // Reverse into buf
    while (i > 0) {
        buf[j++] = tmp[--i];
    }
    buf[j] = '\0';
}
