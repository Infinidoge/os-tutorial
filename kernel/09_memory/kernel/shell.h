#ifndef SHELL_H_
#define SHELL_H_

#define LEN(array) (sizeof(array) / sizeof(array[0]))

typedef struct Command command;

void user_input(const char *input);

#endif // SHELL_H_
