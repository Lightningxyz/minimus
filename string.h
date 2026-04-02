#ifndef STRING_H
#define STRING_H

int strlen(const char *s);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int n);
void strcpy(char *dest, const char *src);
void strcat(char *dest, const char *src);
void *memset(void *ptr, int value, int size);
void *memcpy(void *dest, const void *src, int size);
void int_to_str(int num, char *buf);

#endif
