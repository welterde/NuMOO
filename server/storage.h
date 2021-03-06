#ifndef Storage_h
#define Storage_h 1

#include <string.h>

#include "structures.h"
#include "ref_count.h"

typedef enum Memory_Type {
    M_AST_POOL, M_AST, M_PROGRAM, M_PVAL, M_NETWORK, M_STRING, M_VERBDEF,
    M_LIST, M_PREP, M_PROPDEF, M_OBJECT_TABLE, M_OBJECT, M_FLOAT,
    M_STREAM, M_NAMES, M_ENV, M_TASK, M_PATTERN,

    M_BYTECODES, M_FORK_VECTORS, M_LIT_LIST,
    M_PROTOTYPE, M_CODE_GEN, M_DISASSEMBLE, M_DECOMPILE,

    M_RT_STACK, M_RT_ENV, M_BI_FUNC_DATA, M_VM,

    M_REF_ENTRY, M_REF_TABLE, M_VC_ENTRY, M_VC_TABLE, M_STRING_PTRS,
    M_INTERN_POINTER, M_INTERN_ENTRY, M_INTERN_HUNK,

    M_WAIF, M_WAIF_XTRA,

    Sizeof_Memory_Type

} Memory_Type;

extern char *str_dup(const char *);
extern const char *str_ref(const char *);
extern Var memory_usage(void);

extern void myfree(void *where, Memory_Type type);
extern void *mymalloc(unsigned size, Memory_Type type);
extern void *myrealloc(void *where, unsigned size, Memory_Type type);

static inline void		/* XXX was extern, fix for non-gcc compilers */
free_str(const char *s)
{
    if (delref(s) == 0)
	myfree((void *) s, M_STRING);
}

#ifdef MEMO_STRLEN
/*
 * Using the same mechanism as ref_count.h uses to hide Value ref counts,
 * keep a memozied strlen in the storage with the string.
 */
#define memo_strlen(X)		((void)0, (((int *)(X))[-2]))
#else
#define memo_strlen(X)		strlen(X)

#endif /* MEMO_STRLEN */

#endif				/* Storage_h */
