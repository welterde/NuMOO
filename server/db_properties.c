/*****************************************************************************
 * Routines for manipulating properties on DB objects
 *****************************************************************************/

#include "config.h"
#include "db.h"
#include "db_private.h"
#include "list.h"
#include "storage.h"
#include "utils.h"
#include "waif.h"

Propdef
dbpriv_new_propdef(const char *name)
{
    Propdef newprop;

    newprop.name = str_ref(name);
    newprop.hash = str_hash(name);
    return newprop;
}

int
dbpriv_count_properties(Objid oid)
{
    Object *o;
    int nprops = 0;

    for (o = dbpriv_find_object(oid); o; o = dbpriv_find_object(o->parent))
	nprops += o->propdefs.cur_length;

    return nprops;
}

static int
property_defined_at_or_below(const char *pname, int phash, Objid oid)
{
    /* Return true iff some descendant of OID defines a property named PNAME.
     */
    Objid c;
    Proplist *props = &dbpriv_find_object(oid)->propdefs;
    int length = props->cur_length;
    int i;

    for (i = 0; i < length; i++)
	if (props->l[i].hash == phash
	    && !mystrcasecmp(props->l[i].name, pname))
	    return 1;

    for (c = dbpriv_find_object(oid)->child;
	 c != NOTHING;
	 c = dbpriv_find_object(c)->sibling)
	if (property_defined_at_or_below(pname, phash, c))
	    return 1;

    return 0;
}

static void
insert_prop(Objid oid, int pos, Pval pval)
{
    Pval *new_propval;
    Object *o;
    int i, nprops;

    nprops = dbpriv_count_properties(oid);
    new_propval = (Pval *)mymalloc(nprops * sizeof(Pval), M_PVAL);

    o = dbpriv_find_object(oid);

    free_waif_propdefs((WaifPropdefs*)o->waif_propdefs);
    o->waif_propdefs = NULL;

    for (i = 0; i < pos; i++)
	new_propval[i] = o->propval[i];

    new_propval[pos] = pval;
    new_propval[pos].var = var_ref(pval.var);
    if (new_propval[pos].perms & PF_CHOWN)
	new_propval[pos].owner = o->owner;

    for (i = pos + 1; i < nprops; i++)
	new_propval[i] = o->propval[i - 1];

    if (o->propval)
	myfree(o->propval, M_PVAL);
    o->propval = new_propval;
}

static void
insert_prop_recursively(Objid root, int root_pos, Pval pv)
{
    Objid c;

    insert_prop(root, root_pos, pv);
    pv.var.type = TYPE_CLEAR;	/* do after initial insert_prop so only
				   children will be TYPE_CLEAR */
    for (c = dbpriv_find_object(root)->child;
	 c != NOTHING;
	 c = dbpriv_find_object(c)->sibling) {
	int new_prop_count = dbpriv_find_object(c)->propdefs.cur_length;

	insert_prop_recursively(c, new_prop_count + root_pos, pv);
    }
}

int
db_add_propdef(Objid oid, const char *pname, Var value, Objid owner,
	       unsigned flags)
{
    Object *o;
    Pval pval;
    int i;
    db_prop_handle h;

    h = db_find_property(oid, pname, 0);

    if (h.ptr || property_defined_at_or_below(pname, str_hash(pname), oid))
	return 0;

    o = dbpriv_find_object(oid);
    if (o->propdefs.cur_length == o->propdefs.max_length) {
	Propdef *old_props = o->propdefs.l;
	int new_size = (o->propdefs.max_length == 0
			? 8 : 2 * o->propdefs.max_length);

	o->propdefs.l = (Propdef *)mymalloc(new_size * sizeof(Propdef), M_PROPDEF);
	for (i = 0; i < o->propdefs.max_length; i++)
	    o->propdefs.l[i] = old_props[i];
	o->propdefs.max_length = new_size;

	if (old_props)
	    myfree(old_props, M_PROPDEF);
    }
    o->propdefs.l[o->propdefs.cur_length++] = dbpriv_new_propdef(pname);

    pval.var = value;
    pval.owner = owner;
    pval.perms = flags;

    insert_prop_recursively(oid, o->propdefs.cur_length - 1, pval);

    return 1;
}

static void
rename_prop_recursively(Objid root, const char *old, const char *newbie)
{
    Objid c;
    Object *o = dbpriv_find_object(root);

    if (o->waif_propdefs)
	waif_rename_propdef(o, old, newbie);
    for (c = o->child; c != NOTHING; c = dbpriv_find_object(c)->sibling)
	rename_prop_recursively(c, old, newbie);
}

int
db_rename_propdef(Objid oid, const char *old, const char *newbie)
{
    Proplist *props = &dbpriv_find_object(oid)->propdefs;
    int hash = str_hash(old);
    int count = props->cur_length;
    int i;
    db_prop_handle h;

    for (i = 0; i < count; i++) {
	Propdef p;

	p = props->l[i];
	if (p.hash == hash && !mystrcasecmp(p.name, old)) {
	    if (mystrcasecmp(old, newbie) != 0) {	/* Not changing just the case */
		h = db_find_property(oid, newbie, 0);
		if (h.ptr
		|| property_defined_at_or_below(newbie, str_hash(newbie), oid))
		    return 0;
	    }
	    rename_prop_recursively(oid, props->l[i].name, newbie);
	    free_str(props->l[i].name);
	    props->l[i].name = str_ref(newbie);
	    props->l[i].hash = str_hash(newbie);

	    return 1;
	}
    }

    return 0;
}

static void
remove_prop(Objid oid, int pos)
{
    Pval *new_propval;
    Object *o;
    int i, nprops;

    o = dbpriv_find_object(oid);
    nprops = dbpriv_count_properties(oid);

    free_waif_propdefs((WaifPropdefs*)o->waif_propdefs);
    o->waif_propdefs = NULL;

    free_var(o->propval[pos].var);	/* free deleted property */

    if (nprops) {
	new_propval = (Pval *)mymalloc(nprops * sizeof(Pval), M_PVAL);
	for (i = 0; i < pos; i++)
	    new_propval[i] = o->propval[i];
	for (i = pos; i < nprops; i++)
	    new_propval[i] = o->propval[i + 1];
    } else
	new_propval = 0;

    if (o->propval)
	myfree(o->propval, M_PVAL);
    o->propval = new_propval;
}

static void
remove_prop_recursively(Objid root, int root_pos)
{
    Objid c;

    remove_prop(root, root_pos);
    for (c = dbpriv_find_object(root)->child;
	 c != NOTHING;
	 c = dbpriv_find_object(c)->sibling) {
	int new_prop_count = dbpriv_find_object(c)->propdefs.cur_length;

	remove_prop_recursively(c, new_prop_count + root_pos);
    }
}

int
db_delete_propdef(Objid oid, const char *pname)
{
    Proplist *props = &dbpriv_find_object(oid)->propdefs;
    int hash = str_hash(pname);
    int count = props->cur_length;
    int max = props->max_length;
    int i, j;

    for (i = 0; i < count; i++) {
	Propdef p;

	p = props->l[i];
	if (p.hash == hash && !mystrcasecmp(p.name, pname)) {
	    if (p.name)
		free_str(p.name);

	    if (max > 8 && props->cur_length <= ((max * 3) / 8)) {
		int new_size = max / 2;
		Propdef *new_props;

		new_props = (Propdef *)mymalloc(new_size * sizeof(Propdef), M_PROPDEF);

		for (j = 0; j < i; j++)
		    new_props[j] = props->l[j];
		for (j = i + 1; j < count; j++)
		    new_props[j - 1] = props->l[j];

		myfree(props->l, M_PROPDEF);
		props->l = new_props;
		props->max_length = new_size;
	    } else
		for (j = i + 1; j < count; j++)
		    props->l[j - 1] = props->l[j];

	    props->cur_length--;
	    remove_prop_recursively(oid, i);

	    return 1;
	}
    }

    return 0;
}

int
db_count_propdefs(Objid oid)
{
    return dbpriv_find_object(oid)->propdefs.cur_length;
}

int
db_for_all_propdefs(Objid oid, int (*func) (void *, const char *), void *data)
{
    int i;
    Object *o = dbpriv_find_object(oid);
    int len = o->propdefs.cur_length;

    for (i = 0; i < len; i++)
	if (func(data, o->propdefs.l[i].name))
	    return 1;

    return 0;
}

struct contents_data {
    Var r;
    int i;
};

static int
add_to_list(void *data, Objid c)
{
    struct contents_data *d = (struct contents_data *)data;

    d->i++;
    d->r.v.list[d->i].type = TYPE_OBJ;
    d->r.v.list[d->i].v.obj = c;

    return 0;
}

static void
get_bi_value(db_prop_handle h, Var * value)
{
    Objid oid = *((Objid *) h.ptr);

    switch (h.built_in) {
    case BP_NAME:
	value->type = (var_type)TYPE_STR;
	value->v.str = str_ref(db_object_name(oid));
	break;
    case BP_OWNER:
	value->type = TYPE_OBJ;
	value->v.obj = db_object_owner(oid);
	break;
    case BP_PROGRAMMER:
	value->type = TYPE_INT;
	value->v.num = db_object_has_flag(oid, FLAG_PROGRAMMER);
	break;
    case BP_WIZARD:
	value->type = TYPE_INT;
	value->v.num = db_object_has_flag(oid, FLAG_WIZARD);
	break;
    case BP_R:
	value->type = TYPE_INT;
	value->v.num = db_object_has_flag(oid, FLAG_READ);
	break;
    case BP_W:
	value->type = TYPE_INT;
	value->v.num = db_object_has_flag(oid, FLAG_WRITE);
	break;
    case BP_F:
	value->type = TYPE_INT;
	value->v.num = db_object_has_flag(oid, FLAG_FERTILE);
	break;
    case BP_LOCATION:
	value->type = TYPE_OBJ;
	value->v.obj = db_object_location(oid);
	break;
    case BP_CONTENTS:
	{
	    struct contents_data d;

	    d.r = new_list(db_count_contents(oid));
	    d.i = 0;
	    db_for_all_contents(oid, add_to_list, &d);

	    *value = d.r;
	}
	break;
    default:
	panic("Unknown built-in property in GET_BI_VALUE!");
    }
}

db_prop_handle
db_find_property(Objid oid, const char *name, Var * value)
{
    static struct {
	const char *name;
	enum bi_prop prop;
	int hash;
    } ptable[] = {
	{
	    "name", BP_NAME, 0
	},
	{
	    "owner", BP_OWNER, 0
	},
	{
	    "programmer", BP_PROGRAMMER, 0
	},
	{
	    "wizard", BP_WIZARD, 0
	},
	{
	    "r", BP_R, 0
	},
	{
	    "w", BP_W, 0
	},
	{
	    "f", BP_F, 0
	},
	{
	    "location", BP_LOCATION, 0
	},
	{
	    "contents", BP_CONTENTS, 0
	}
    };
    static int ptable_init = 0;
    int i, n;
    db_prop_handle h;
    int hash = str_hash(name);
    Object *o;

    if (!ptable_init) {
	for (i = 0; i < Arraysize(ptable); i++)
	    ptable[i].hash = str_hash(ptable[i].name);
	ptable_init = 1;
    }
    for (i = 0; i < Arraysize(ptable); i++) {
	if (ptable[i].hash == hash && !mystrcasecmp(name, ptable[i].name)) {
	    static Objid ret;

	    ret = oid;
	    h.built_in = ptable[i].prop;
	    h.definer = NOTHING;
	    h.ptr = &ret;
	    if (value)
		get_bi_value(h, value);
	    return h;
	}
    }

    h.built_in = BP_NONE;
    n = 0;
    for (o = dbpriv_find_object(oid); o; o = dbpriv_find_object(o->parent)) {
	Proplist *props = &(o->propdefs);
	Propdef *defs = props->l;
	int length = props->cur_length;

	for (i = 0; i < length; i++, n++) {
	    if (defs[i].hash == hash
		&& !mystrcasecmp(defs[i].name, name)) {
		Pval *prop;

		h.definer = o->id;
		o = dbpriv_find_object(oid);
                h.ptr = o->propval + n;
		prop = (Pval *)h.ptr;

		if (value) {
		    while (prop->var.type == TYPE_CLEAR) {
			n -= o->propdefs.cur_length;
			o = dbpriv_find_object(o->parent);
			prop = o->propval + n;
		    }
		    *value = prop->var;
		}
		return h;
	    }
	}
    }

    h.ptr = 0;
    return h;
}

Var
db_property_value(db_prop_handle h)
{
    Var value;

    if (h.built_in)
	get_bi_value(h, &value);
    else {
	Pval *prop = (Pval *)h.ptr;

	value = prop->var;
    }

    return value;
}

void
db_set_property_value(db_prop_handle h, Var value)
{
    if (!h.built_in) {
	Pval *prop = (Pval *)h.ptr;

	free_var(prop->var);
	prop->var = value;
    } else {
	Objid oid = *((Objid *) h.ptr);
	db_object_flag flag;

	switch (h.built_in) {
	case BP_NAME:
	    if (value.type != TYPE_STR)
		goto complain;
	    db_set_object_name(oid, value.v.str);
	    break;
	case BP_OWNER:
	    if (value.type != TYPE_OBJ)
		goto complain;
	    db_set_object_owner(oid, value.v.obj);
	    break;
	case BP_PROGRAMMER:
	    flag = FLAG_PROGRAMMER;
	    goto finish_flag;
	case BP_WIZARD:
	    flag = FLAG_WIZARD;
	    goto finish_flag;
	case BP_R:
	    flag = FLAG_READ;
	    goto finish_flag;
	case BP_W:
	    flag = FLAG_WRITE;
	    goto finish_flag;
	case BP_F:
	    flag = FLAG_FERTILE;
	  finish_flag:
	    if (is_true(value))
		db_set_object_flag(oid, flag);
	    else
		db_clear_object_flag(oid, flag);
	    free_var(value);
	    break;
	case BP_LOCATION:
	case BP_CONTENTS:
	  complain:
	    panic("Inappropriate value in DB_SET_PROPERTY_VALUE!");
	    break;
	default:
	    panic("Unknown built-in property in DB_SET_PROPERTY_VALUE!");
	}
    }
}

Objid
db_property_owner(db_prop_handle h)
{
    if (h.built_in) {
	panic("Built-in property in DB_PROPERTY_OWNER!");
	return NOTHING;
    } else {
	Pval *prop = (Pval *)h.ptr;

	return prop->owner;
    }
}

void
db_set_property_owner(db_prop_handle h, Objid oid)
{
    if (h.built_in)
	panic("Built-in property in DB_SET_PROPERTY_OWNER!");
    else {
	Pval *prop = (Pval *)h.ptr;

	prop->owner = oid;
    }
}

unsigned
db_property_flags(db_prop_handle h)
{
    if (h.built_in) {
	panic("Built-in property in DB_PROPERTY_FLAGS!");
	return 0;
    } else {
	Pval *prop = (Pval *)h.ptr;

	return prop->perms;
    }
}

void
db_set_property_flags(db_prop_handle h, unsigned flags)
{
    if (h.built_in)
	panic("Built-in property in DB_SET_PROPERTY_FLAGS!");
    else {
	Pval *prop = (Pval *)h.ptr;

	prop->perms = flags;
    }
}

int
db_property_allows(db_prop_handle h, Objid progr, db_prop_flag flag)
{
    return ((db_property_flags(h) & flag)
	    || progr == db_property_owner(h)
	    || is_wizard(progr));
}

static void
fix_props(Objid oid, int parent_local, int old, int newbie, int common)
{
    Object *me = dbpriv_find_object(oid);
    Object *parent = dbpriv_find_object(me->parent);
    Pval *new_propval;
    int local = parent_local;
    int i;
    Objid c;

    /* This will invalidate waif_propdefs */
    free_waif_propdefs((WaifPropdefs*)me->waif_propdefs);
    me->waif_propdefs = NULL;

    local += me->propdefs.cur_length;

    for (i = local; i < local + old; i++)
	free_var(me->propval[i].var);

    if (local + newbie + common != 0) {
	new_propval = (Pval *)mymalloc((local + newbie + common) * sizeof(Pval), M_PVAL);
	for (i = 0; i < local; i++)
	    new_propval[i] = me->propval[i];
	for (i = 0; i < newbie; i++) {
	    Pval pv;

	    pv = parent->propval[parent_local + i];
	    new_propval[local + i] = pv;
	    new_propval[local + i].var.type = TYPE_CLEAR;
	    if (pv.perms & PF_CHOWN)
		new_propval[local + i].owner = me->owner;
	}
	for (i = 0; i < common; i++)
	    new_propval[local + newbie + i] = me->propval[local + old + i];
    } else
	new_propval = 0;

    if (me->propval)
	myfree(me->propval, M_PVAL);
    me->propval = new_propval;

    for (c = me->child; c != NOTHING; c = dbpriv_find_object(c)->sibling)
	fix_props(c, local, old, newbie, common);
}

int
dbpriv_check_properties_for_chparent(Objid oid, Objid new_parent)
{
    Object *o;
    int i;

    for (o = dbpriv_find_object(new_parent);
	 o;
	 o = dbpriv_find_object(o->parent)) {
	Proplist *props = &o->propdefs;

	for (i = 0; i < props->cur_length; i++)
	    if (property_defined_at_or_below(props->l[i].name,
					     props->l[i].hash,
					     oid))
		return 0;
    }

    return 1;
}

void
dbpriv_fix_properties_after_chparent(Objid oid, Objid old_parent)
{
    Objid o1, o2, common, new_parent;
    int common_props, old_props, new_props;

    /* Find the nearest common ancestor between old & new parent */
    new_parent = db_object_parent(oid);
    common = NOTHING;
    for (o1 = new_parent; o1 != NOTHING; o1 = db_object_parent(o1))
	for (o2 = old_parent; o2 != NOTHING; o2 = db_object_parent(o2))
	    if (o1 == o2) {
		common = o1;
		goto endouter;
	    }
  endouter:

    if (common != NOTHING)
	common_props = dbpriv_count_properties(common);
    else
	common_props = 0;

    old_props = dbpriv_count_properties(old_parent) - common_props;
    new_props = dbpriv_count_properties(new_parent) - common_props;

    fix_props(oid, 0, old_props, new_props, common_props);
}
