#ifndef Sym_Table_h
#define Sym_Table_h 1

#include "config.h"
#include "version.h"

typedef struct {
    unsigned max_size;
    unsigned size;
    const char **names;
} Names;

extern Names *new_builtin_names(DB_Version);
extern int first_user_slot(DB_Version);
extern unsigned find_or_add_name(Names **, const char *);
extern int find_name(Names *, const char *);
extern void free_names(Names *);

/* Environment slots for built-in variables */
#define SLOT_NUM	0
#define SLOT_OBJ	1
#define SLOT_STR	2
#define SLOT_LIST	3
#define SLOT_ERR	4
#define SLOT_PLAYER	5
#define SLOT_THIS	6
#define SLOT_CALLER	7
#define SLOT_VERB	8
#define SLOT_ARGS	9
#define SLOT_ARGSTR	10
#define SLOT_DOBJ	11
#define SLOT_DOBJSTR	12
#define SLOT_PREPSTR	13
#define SLOT_IOBJ	14
#define SLOT_IOBJSTR	15

/* Added in DBV_Float: */
#define SLOT_INT	16
#define SLOT_FLOAT	17

#endif				/* !Sym_Table_h */
