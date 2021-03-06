#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "db.h"
#include "exceptions.h"
#include "structures.h"
#include "match.h"
#include "parse_cmd.h"
#include "storage.h"
#include "unparse.h"
#include "utils.h"

static Var *
aliases(Objid oid)
{
    Var value;
    db_prop_handle h;

    h = db_find_property(oid, "aliases", &value);
    if (!h.ptr || value.type != TYPE_LIST) {
	/* Simulate a pointer to an empty list */
	return &zero;
    } else
	return value.v.list;
}

struct match_data {
    int lname;
    const char *name;
    Objid exact, partial;
};

static int
match_proc(void *data, Objid oid)
{
    struct match_data *d = (struct match_data *)data;
    Var *names = aliases(oid);
    int i;
    const char *name;

    for (i = 0; i <= names[0].v.num; i++) {
	if (i == 0)
	    name = db_object_name(oid);
	else if (names[i].type != TYPE_STR)
	    continue;
	else
	    name = names[i].v.str;

	if (!mystrncasecmp(name, d->name, d->lname)) {
	    if (name[d->lname] == '\0') {	/* exact match */
		if (d->exact == NOTHING || d->exact == oid)
		    d->exact = oid;
		else
		    return 1;
	    } else {		/* partial match */
		if (d->partial == FAILED_MATCH || d->partial == oid)
		    d->partial = oid;
		else
		    d->partial = AMBIGUOUS;
	    }
	}
    }

    return 0;
}

static Objid
match_contents(Objid player, const char *name)
{
    Objid loc;
    int step;
    Objid oid;
    struct match_data d;

    d.lname = strlen(name);
    d.name = name;
    d.exact = NOTHING;
    d.partial = FAILED_MATCH;

    if (!valid(player))
	return FAILED_MATCH;
    loc = db_object_location(player);

    for (oid = player, step = 0; step < 2; oid = loc, step++) {
	if (!valid(oid))
	    continue;
	if (db_for_all_contents(oid, match_proc, &d))
	    /* We only abort the enumeration for exact ambiguous matches... */
	    return AMBIGUOUS;
    }

    if (d.exact != NOTHING)
	return d.exact;
    else
	return d.partial;
}

Objid
match_object(Objid player, const char *name)
{
    if (name[0] == '\0')
	return NOTHING;
    if (name[0] == '#') {
	char *p;
	Objid r = strtol(name + 1, &p, 10);

	if (*p != '\0' || !valid(r))
	    return FAILED_MATCH;
	return r;
    }
    if (!valid(player))
	return FAILED_MATCH;
    if (!mystrcasecmp(name, "me"))
	return player;
    if (!mystrcasecmp(name, "here"))
	return db_object_location(player);
    return match_contents(player, name);
}
