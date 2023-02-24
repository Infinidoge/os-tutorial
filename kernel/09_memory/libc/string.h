#ifndef STRINGS_H
#define STRINGS_H

#include "../cpu/types.h"

void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);
void reverse(char s[]);
int strlen(const char s[]);
void backspace(char s[]);
void append(char s[], char n);
int strcmp(const char s1[], const char s2[]);
bool strbeginswith(const char s1[], const char s2[], int *rest);

#endif
