/*****************************************************************************
 * Private interface for internal communication in the DB implementation
 *****************************************************************************/

#ifndef DB_PRIVATE_h
#define DB_PRIVATE_h

#include "config.h"
#include "exceptions.h"
#include "program.h"
#include "structures.h"

typedef struct Verbdef Verbdef;

struct Verbdef {
    const char *name;
    Program *program;
    Objid owner;
    short perms;
    short prep;
    Verbdef *next;
};

typedef struct Proplist Proplist;
typedef struct Propdef Propdef;

struct Propdef {
    const char *name;
    int hash;
};

struct Proplist {
    int max_length;
    int cur_length;
    Propdef *l;
};

typedef struct Pval {
    Var var;
    Objid owner;
    short perms;
} Pval;

typedef struct Object {
    Objid id;
    Objid owner;
    Objid location;
    Objid contents;
    Objid next;

    Objid parent;
    Objid child;
    Objid sibling;


    const char *name;
    int flags;

    Verbdef *verbdefs;
    Proplist propdefs;
    Pval *propval;

    void *waif_propdefs;
} Object;

/*********** Verb cache support ***********/

#define VERB_CACHE 1

#ifdef VERB_CACHE

/* Whenever anything is modified that could influence callable verb
 * lookup, this function must be called.
 */
extern void db_priv_affected_callable_verb_lookup(void);

#else /* no cache */
#define db_priv_affected_callable_verb_lookup() 
#endif

/*********** Objects ***********/

extern void dbpriv_set_all_users(Var);
				/* Initialize the list returned by
				 * db_all_users().
				 */

extern Object *dbpriv_new_object(void);
				/* Creates a new object, assigning it a number,
				 * but doesn't fill in any of the fields other
				 * than `id'.
				 */

extern void dbpriv_new_recycled_object(void);
				/* Does the equivalent of creating and
				 * destroying an object, with the net effect of
				 * using up the next available object number.
				 */

extern Object *dbpriv_find_object(Objid);
				/* Returns 0 if given object is not valid.
				 */

/*********** Properties ***********/

extern Propdef dbpriv_new_propdef(const char *name);

extern int dbpriv_count_properties(Objid);

extern int dbpriv_check_properties_for_chparent(Objid oid,
						Objid new_parent);
				/* Return true iff NEW_PARENT defines no
				 * properties that are also defined by either
				 * OID or any of OID's descendants.
				 */

extern void dbpriv_fix_properties_after_chparent(Objid oid,
						 Objid old_parent);
				/* OID has just had its parent changed away
				 * from OLD_PARENT.  Fix up the properties of
				 * OID and its descendants, removing obsolete
				 * ones and adding clear new ones, as
				 * appropriate for its new parent.
				 */

/*********** Verbs ***********/

extern void dbpriv_build_prep_table(void);
				/* Should be called once near the beginning of
				 * the world, to initialize the
				 * prepositional-phrase matching table.
				 */

/*********** DBIO ***********/

extern Exception dbpriv_dbio_failed;
				/* Raised by DBIO in case of failure (e.g.,
				 * running out of disk space for the dump).
				 */

extern void dbpriv_set_dbio_input(FILE *);
extern void dbpriv_set_dbio_output(FILE *);

#endif /* DB_PRIVATE_h */
