#ifndef Structures_h
#define Structures_h 1

#include <stdio.h>
#include "options.h"
#include "config.h"

#define MAXINT	((Num) 9223372036854775807LL)
#define MAXOBJ	((Objid) MAXINT)

/* Note: it's a pretty hard assumption in MOO that integers and objects
   are the same data type. */
typedef int64_t Num;
#define PRIdN	PRId64
#define SCNdN	SCNd64
typedef Num Objid;

/*
 * Special Objid's
 */
#define SYSTEM_OBJECT	0
#define NOTHING		-1
#define AMBIGUOUS	-2
#define FAILED_MATCH	-3

/* Do not reorder or otherwise modify this list, except to add new elements at
 * the end, since the order here defines the numeric equivalents of the error
 * values, and those equivalents are both DB-accessible knowledge and stored in
 * raw form in the DB.
 */
enum error {
    E_NONE, E_TYPE, E_DIV, E_PERM, E_PROPNF, E_VERBNF, E_VARNF, E_INVIND,
    E_RECMOVE, E_MAXREC, E_RANGE, E_ARGS, E_NACC, E_INVARG, E_QUOTA, E_FLOAT
};

/* Do not reorder or otherwise modify this list, except to add new elements at
 * the end, since the order here defines the numeric equivalents of the type
 * values, and those equivalents are both DB-accessible knowledge and stored in
 * raw form in the DB.
 */
typedef enum {
    TYPE_INT, TYPE_OBJ, _TYPE_STR, TYPE_ERR, _TYPE_LIST, /* user-visible */
    TYPE_CLEAR,			/* in clear properties' value slot */
    TYPE_NONE,			/* in uninitialized MOO variables */
    TYPE_CATCH,			/* on-stack marker for an exception handler */
    TYPE_FINALLY,		/* on-stack marker for a TRY-FINALLY clause */
    _TYPE_FLOAT,		/* floating-point number; user-visible */
    _TYPE_WAIF			/* lightweight object; user-visible */
} var_type;

/* Types which have external data should be marked with the TYPE_COMPLEX_FLAG
 * so that free_var/var_ref/var_dup can recognize them easily.  This flag is
 * only set in memory.  The original _TYPE values are used in the database
 * file and returned to verbs calling typeof().  This allows the inlines to
 * be extremely cheap (both in space and time) for simple types like oids
 * and ints.
 */
#define TYPE_DB_MASK		0x7f
#define TYPE_COMPLEX_FLAG	0x80

#define TYPE_STR		(_TYPE_STR | TYPE_COMPLEX_FLAG)
#define TYPE_FLOAT		(_TYPE_FLOAT)
#define TYPE_LIST		(_TYPE_LIST | TYPE_COMPLEX_FLAG)
#define TYPE_WAIF		(_TYPE_WAIF | TYPE_COMPLEX_FLAG)

#define TYPE_ANY ((var_type) -1)	/* wildcard for use in declaring built-ins */
#define TYPE_NUMERIC ((var_type) -2)	/* wildcard for (integer or float) */

typedef struct Var Var;

struct WaifPropdefs;

/* Try to make struct Waif fit into 32 bytes with this mapsz.  These bytes
 * are probably "free" (from a powers-of-two allocator) and we can use them
 * to save lots of space.  With 64bit addresses I think the right value is 8.
 * If checkpoints are unforked, save space for an index used while saving.
 * Otherwise we can alias propdefs and clobber it in the child.
 */
#ifdef UNFORKED_CHECKPOINTS
#define WAIF_MAPSZ	2
#else
#define WAIF_MAPSZ	3
#endif

typedef struct Waif {
	Objid			class;
	Objid			owner;
	struct WaifPropdefs	*propdefs;
	Var			*propvals;
	unsigned long		map[WAIF_MAPSZ];
#ifdef UNFORKED_CHECKPOINTS
	unsigned long		waif_save_index;
#else
#define waif_save_index		map[0]
#endif
} Waif;

struct Var {
    union {
	const char *str;	/* STR */
	Num num;		/* NUM, CATCH, FINALLY */
	Objid obj;		/* OBJ */
	enum error err;		/* ERR */
	Var *list;		/* LIST */
	double fnum;		/* FLOAT */
	Waif *waif;		/* WAIF */
    } v;
    var_type type;
};

extern Var zero;		/* useful constant */

#endif				/* !Structures_h */
