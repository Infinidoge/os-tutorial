#ifndef FUNCTION_H
#define FUNCTION_H

#include "../cpu/types.h"

/* Sometimes we want to keep parameters to a function for later use
 * and this is a solution to avoid the 'unused parameter' compiler warning */
#define UNUSED(x) (void)(x)

// Predicates: Functions that take an input and produce a boolean
typedef bool *predicate(void *input);

// Functions: Functions that take input and produce an output
typedef void *(*function0)();
typedef void *(*function1)(void *input1);
typedef void *(*function2)(void *input1, void *input2);
typedef void *(*function3)(void *input1, void *input2, void *input3);
typedef void *(*function4)(void *input1, void *input2, void *input3, void *input4);
typedef void *(*function5)(void *input1, void *input2, void *input3, void *input4, void *input5);

// Actions: Functions that take input and do not produce an output
typedef void (*action0)();
typedef void (*action1)(void *input1);
typedef void (*action2)(void *input1, void *input2);
typedef void (*action3)(void *input1, void *input2, void *input3);
typedef void (*action4)(void *input1, void *input2, void *input3, void *input4);
typedef void (*action5)(void *input1, void *input2, void *input3, void *input4, void *input5);

#endif
