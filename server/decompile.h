#include "ast.h"
#include "program.h"

extern Stmt *decompile_program(Program * program, int vector);
extern int find_line_number(Program * program, int vector, int pc);
