#ifndef WAIF_h
#define WAIF_h

#define WAIF_PROP_PREFIX	':'
#define WAIF_VERB_PREFIX	':'

#ifdef WAIF_DICT
#define WAIF_INDEX_VERB ":_index"
#define WAIF_INDEXSET_VERB ":_set_index"
#endif /* WAIF_DICT */

#include "db_private.h"

typedef struct WaifPropdefs {
	int		refcount;
	int		length;
	struct Propdef	defs[1];
} WaifPropdefs;

extern void free_waif(Waif *);
extern Waif *dup_waif(Waif *);
extern enum error waif_get_prop(Waif *, const char *, Var *, Objid progr);
extern enum error waif_put_prop(Waif *, const char *, Var, Objid progr);
extern int waif_bytes(Waif *);
extern void waif_before_saving();
extern void waif_after_saving();
extern void waif_before_loading();
extern void waif_after_loading();
extern void write_waif(Var);
extern Var read_waif();
extern void free_waif_propdefs(WaifPropdefs *);                                 
extern void waif_rename_propdef(Object *, const char *, const char *);

#endif /* WAIF_h */
