#include <stdio.h>

#include "ast.h"
#include "config.h"
#include "exceptions.h"
#include "log.h"
#include "storage.h"
#include "structures.h"
#include "sym_table.h"
#include "utils.h"
#include "version.h"

static Names *
new_names(unsigned max_size)
{
    Names *names = (Names *)mymalloc(sizeof(Names), M_NAMES);

    names->names = mymalloc(sizeof(char *) * max_size, M_NAMES);
    names->max_size = max_size;
    names->size = 0;

    return names;
}

static Names *
copy_names(Names * old)
{
    Names *newbie = new_names(old->size);
    unsigned i;

    newbie->size = old->size;
    for (i = 0; i < newbie->size; i++)
	newbie->names[i] = str_ref(old->names[i]);

    return newbie;
}

int
first_user_slot(DB_Version version)
{
    int count = 16;		/* DBV_Prehistory count */

    if (version >= DBV_Float)
	count += 2;

    return count;
}

Names *
new_builtin_names(DB_Version version)
{
    static Names *builtins[Num_DB_Versions];

    if (builtins[version] == 0) {
	Names *bi = new_names(first_user_slot(version));

	builtins[version] = bi;
	bi->size = bi->max_size;

	bi->names[SLOT_NUM] = str_dup("NUM");
	bi->names[SLOT_OBJ] = str_dup("OBJ");
	bi->names[SLOT_STR] = str_dup("STR");
	bi->names[SLOT_LIST] = str_dup("LIST");
	bi->names[SLOT_ERR] = str_dup("ERR");
	bi->names[SLOT_PLAYER] = str_dup("player");
	bi->names[SLOT_THIS] = str_dup("this");
	bi->names[SLOT_CALLER] = str_dup("caller");
	bi->names[SLOT_VERB] = str_dup("verb");
	bi->names[SLOT_ARGS] = str_dup("args");
	bi->names[SLOT_ARGSTR] = str_dup("argstr");
	bi->names[SLOT_DOBJ] = str_dup("dobj");
	bi->names[SLOT_DOBJSTR] = str_dup("dobjstr");
	bi->names[SLOT_PREPSTR] = str_dup("prepstr");
	bi->names[SLOT_IOBJ] = str_dup("iobj");
	bi->names[SLOT_IOBJSTR] = str_dup("iobjstr");

	if (version >= DBV_Float) {
	    bi->names[SLOT_INT] = str_dup("INT");
	    bi->names[SLOT_FLOAT] = str_dup("FLOAT");
	}
    }
    return copy_names(builtins[version]);
}

int
find_name(Names * names, const char *str)
{
    unsigned i;

    for (i = 0; i < names->size; i++)
	if (!mystrcasecmp(names->names[i], str))
	    return i;
    return -1;
}

unsigned
find_or_add_name(Names ** names, const char *str)
{
    unsigned i;

    for (i = 0; i < (*names)->size; i++)
	if (!mystrcasecmp((*names)->names[i], str)) {	/* old name */
	    return i;
	}
    if ((*names)->size == (*names)->max_size) {
	unsigned old_max = (*names)->max_size;
	Names *newbie = new_names(old_max * 2);
	unsigned i;

	for (i = 0; i < old_max; i++)
	    newbie->names[i] = (*names)->names[i];
	newbie->size = old_max;
	myfree((*names)->names, M_NAMES);
	myfree(*names, M_NAMES);
	*names = newbie;
    }
    (*names)->names[(*names)->size] = str_dup(str);
    return (*names)->size++;
}

void
free_names(Names * names)
{
    unsigned i;

    for (i = 0; i < names->size; i++)
	free_str(names->names[i]);
    myfree(names->names, M_NAMES);
    myfree(names, M_NAMES);
}
