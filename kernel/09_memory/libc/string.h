#ifndef STRINGS_H
#define STRINGS_H

#include "../cpu/types.h"

void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);
void uint_to_ascii(unsigned int n, char str[]);
void uhex_to_ascii(unsigned int n, char str[]);
void bin_to_ascii(unsigned int n, char str[]);
void bin_to_ascii_padded(unsigned int n, char str[], int padding);
void reverse(char s[]);
int strlen(const char s[]);
void backspace(char s[]);
void append(char s[], char n);
int strcmp(const char s1[], const char s2[]);
bool strbeginswith(const char s1[], const char s2[], int *rest);

#endif
