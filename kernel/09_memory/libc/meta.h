#ifndef META_H_
#define META_H_

#include <stddef.h>

extern size_t _etext;
extern size_t _edata;
extern size_t _end;

#define ETEXT (size_t) & _etext
#define EDATA (size_t) & _edata
#define END (size_t) & _end

#endif // META_H_
