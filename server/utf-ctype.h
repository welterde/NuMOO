#ifndef Utf_Ctype_H
#define Utf_Ctype_H 1

#include "config.h"

extern int my_tolower(int);
extern int my_toupper(int);

extern int my_isdigit(int);
extern int my_digitval(int);

extern int my_isspace(int);

extern int my_is_xid_start(int);
extern int my_is_xid_cont(int);

extern int my_is_printable(int);

#define my_isascii(x) ((unsigned int)(x) < 127)

#endif				/* !Utf_Ctype_H */
