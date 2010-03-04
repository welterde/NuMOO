#ifndef Unparse_h
#define Unparse_h 1

#include <stdio.h>

#include "config.h"
#include "program.h"
#include "structures.h"

typedef void (*Unparser_Receiver) (void *, const char *);

extern void unparse_program(Program *, Unparser_Receiver, void *,
			    int fully_parenthesize,
			    int indent_lines, int f_index);

extern void unparse_to_file(FILE * fp, Program *,
			    int fully_parenthesize,
			    int indent_lines, int f_index);
extern void unparse_to_stderr(Program *, int fully_parenthesize,
			      int indent_lines, int f_index);

extern const char *error_name(enum error);	/* E_NONE -> "E_NONE" */
extern const char *unparse_error(enum error);	/* E_NONE -> "No error" */

#endif
