#ifndef Parser_h
#define Parser_h 1

#include "config.h"
#include "program.h"
#include "version.h"

typedef struct {
    void (*error) (void *, const char *);
    void (*warning) (void *, const char *);
    int (*getch) (void *);
} Parser_Client;

extern Program *parse_program(DB_Version, Parser_Client, void *);
extern Program *parse_list_as_program(Var code, Var * errors);

#endif
