#include "string.h"

/**
 * K&R implementation
 */
void int_to_ascii(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0)
        str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

const char hex[] = "0123456789abcdef";

void hex_to_ascii(int n, char str[]) {
    int sign;

    if ((sign = n) < 0)
        n = -n;

    int i = 0;

    do {
        str[i++] = hex[n % 16];
    } while ((n >>= 4) > 0);

    str[i++] = 'x';
    str[i++] = '0';

    if (sign < 0)
        str[i++] = '-';

    str[i] = '\0';

    reverse(str);
}

void uint_to_ascii(unsigned int n, char str[]) {
    int i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    str[i] = '\0';

    reverse(str);
}

void uhex_to_ascii(unsigned int n, char str[]) {
    int i = 0;

    do {
        str[i++] = hex[n % 16];
    } while ((n >>= 4) > 0);

    str[i++] = 'x';
    str[i++] = '0';

    str[i] = '\0';

    reverse(str);
}

void bin_to_ascii(unsigned int n, char str[]) {
    int i = 0;
    do {
        str[i++] = n % 2 == 1 ? '1' : '0';
    } while ((n >>= 1) > 0);

    str[i++] = 'b';
    str[i++] = '0';

    str[i] = '\0';

    reverse(str);
}

void bin_to_ascii_padded(unsigned int n, char str[], int padding) {
    int i = 0;
    do {
        str[i++] = n % 2 == 1 ? '1' : '0';
    } while ((n >>= 1) > 0);

    while (i < padding) {
        str[i++] = '0';
    }

    str[i++] = 'b';
    str[i++] = '0';

    str[i] = '\0';

    reverse(str);
}

/* K&R */
void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* K&R */
int strlen(const char s[]) {
    int i = 0;
    while (s[i] != '\0')
        ++i;
    return i;
}

void append(char s[], char n) {
    int len = strlen(s);
    s[len] = n;
    s[len + 1] = '\0';
}

void backspace(char s[]) {
    int len = strlen(s);
    s[len - 1] = '\0';
}

/* K&R
 * Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2 */
int strcmp(const char s1[], const char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0')
            return 0;
    }
    return s1[i] - s2[i];
}

bool strbeginswith(const char s1[], const char s2[], int *rest) {
    int i;
    for (i = 0; s2[i] != '\0'; i++) {
        if (s1[i] != s2[i])
            return false;
    }

    *rest = i;

    return true;
}
