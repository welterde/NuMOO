#ifndef Program_H
#define Program_H

#include "structures.h"
#include "version.h"

typedef uint8_t Byte;

typedef struct {
    Byte numbytes_label, numbytes_literal, numbytes_fork, numbytes_var_name,
     numbytes_stack;
    Byte *vector;
    unsigned size;
    unsigned max_stack;
} Bytecodes;

typedef struct {
    DB_Version version;
    unsigned first_lineno;
    unsigned ref_count;

    Bytecodes main_vector;

    unsigned num_literals;
    Var *literals;

    unsigned fork_vectors_size;
    Bytecodes *fork_vectors;

    unsigned num_var_names;
    const char **var_names;

    unsigned cached_lineno;
    unsigned cached_lineno_pc;
    int cached_lineno_vec;
} Program;

#define MAIN_VECTOR 	-1	/* As opposed to an index into fork_vectors */

extern Program *new_program(void);
extern Program *null_program(void);
extern Program *program_ref(Program *);
extern int program_bytes(Program *);
extern void free_program(Program *);

#endif				/* !Program_H */
