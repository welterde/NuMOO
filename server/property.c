#include "db.h"
#include "functions.h"
#include "list.h"
#include "storage.h"
#include "utils.h"

struct prop_data {
    Var r;
    int i;
};

static int
add_to_list(void *data, const char *prop_name)
{
    struct prop_data *d = data;

    d->i++;
    d->r.v.list[d->i].type = TYPE_STR;
    d->r.v.list[d->i].v.str = str_ref(prop_name);

    return 0;
}

static package
bf_properties(Var arglist, Byte next, void *vdata, Objid progr)
{				/* (object) */
    Objid oid = arglist.v.list[1].v.obj;

    free_var(arglist);

    if (!valid(oid))
	return make_error_pack(E_INVARG);
    else if (!db_object_allows(oid, progr, FLAG_READ))
	return make_error_pack(E_PERM);
    else {
	struct prop_data d;

	d.r = new_list(db_count_propdefs(oid));
	d.i = 0;
	db_for_all_propdefs(oid, add_to_list, &d);

	return make_var_pack(d.r);
    }
}


static package
bf_prop_info(Var arglist, Byte next, void *vdata, Objid progr)
{				/* (object, prop-name) */
    Objid oid = arglist.v.list[1].v.obj;
    const char *pname = arglist.v.list[2].v.str;
    db_prop_handle h;
    Var r;
    unsigned flags;
    char *s;

    if (!valid(oid)) {
	free_var(arglist);
	return make_error_pack(E_INVARG);
    }
    h = db_find_property(oid, pname, 0);
    free_var(arglist);

    if (!h.ptr || h.built_in)
	return make_error_pack(E_PROPNF);
    else if (!db_property_allows(h, progr, PF_READ))
	return make_error_pack(E_PERM);

    r = new_list(2);
    r.v.list[1].type = TYPE_OBJ;
    r.v.list[1].v.obj = db_property_owner(h);
    r.v.list[2].type = TYPE_STR;
    r.v.list[2].v.str = s = str_dup("xxx");
    flags = db_property_flags(h);
    if (flags & PF_READ)
	*s++ = 'r';
    if (flags & PF_WRITE)
	*s++ = 'w';
    if (flags & PF_CHOWN)
	*s++ = 'c';
    *s = '\0';

    return make_var_pack(r);
}

static enum error
validate_prop_info(Var v, Objid * owner, unsigned *flags, const char **name)
{
    const char *s;
    int len = (v.type == TYPE_LIST ? v.v.list[0].v.num : 0);

    if (!((len == 2 || len == 3)
	  && v.v.list[1].type == TYPE_OBJ
	  && v.v.list[2].type == TYPE_STR
	  && (len == 2 || v.v.list[3].type == TYPE_STR)))
	return E_TYPE;

    *owner = v.v.list[1].v.obj;
    if (!valid(*owner))
	return E_INVARG;

    for (*flags = 0, s = v.v.list[2].v.str; *s; s++) {
	switch (*s) {
	case 'r':
	case 'R':
	    *flags |= PF_READ;
	    break;
	case 'w':
	case 'W':
	    *flags |= PF_WRITE;
	    break;
	case 'c':
	case 'C':
	    *flags |= PF_CHOWN;
	    break;
	default:
	    return E_INVARG;
	}
    }

    if (len == 2)
	*name = 0;
    else
	*name = v.v.list[3].v.str;

    return E_NONE;
}

static enum error
set_prop_info(Objid oid, const char *pname, Var info, Objid progr)
{
    Objid new_owner;
    unsigned new_flags;
    const char *new_name;
    enum error e;
    db_prop_handle h;

    if (!valid(oid))
	e = E_INVARG;
    else
	e = validate_prop_info(info, &new_owner, &new_flags, &new_name);

    if (e != E_NONE)
	return e;

    h = db_find_property(oid, pname, 0);

    if (!h.ptr || h.built_in)
	return E_PROPNF;
    else if (!db_property_allows(h, progr, PF_WRITE)
	     || (!is_wizard(progr) && db_property_owner(h) != new_owner))
	return E_PERM;

    if (new_name) {
	if (!db_rename_propdef(oid, pname, new_name))
	    return E_INVARG;

	h = db_find_property(oid, new_name, 0);
    }
    db_set_property_owner(h, new_owner);
    db_set_property_flags(h, new_flags);

    return E_NONE;
}

static package
bf_set_prop_info(Var arglist, Byte next, void *vdata, Objid progr)
{				/* (object, prop-name, {owner, perms [, new-name]}) */
    Objid oid = arglist.v.list[1].v.obj;
    const char *pname = arglist.v.list[2].v.str;
    Var info = arglist.v.list[3];
    enum error e = set_prop_info(oid, pname, info, progr);

    free_var(arglist);
    if (e == E_NONE)
	return no_var_pack();
    else
	return make_error_pack(e);
}

static package
bf_add_prop(Var arglist, Byte next, void *vdata, Objid progr)
{				/* (object, prop-name, initial-value, initial-info) */
    Objid oid = arglist.v.list[1].v.obj;
    const char *pname = arglist.v.list[2].v.str;
    Var value = arglist.v.list[3];
    Var info = arglist.v.list[4];
    Objid owner;
    unsigned flags;
    const char *new_name;
    enum error e;

    if ((e = validate_prop_info(info, &owner, &flags, &new_name)) != E_NONE);	/* Already failed */
    else if (new_name)
	e = E_TYPE;
    else if (!valid(oid))
	e = E_INVARG;
    else if (!db_object_allows(oid, progr, FLAG_WRITE)
	     || (progr != owner && !is_wizard(progr)))
	e = E_PERM;
    else if (!db_add_propdef(oid, pname, value, owner, flags))
	e = E_INVARG;

    free_var(arglist);
    if (e == E_NONE)
	return no_var_pack();
    else
	return make_error_pack(e);
}

static package
bf_delete_prop(Var arglist, Byte next, void *vdata, Objid progr)
{				/* (object, prop-name) */
    Objid oid = arglist.v.list[1].v.obj;
    const char *pname = arglist.v.list[2].v.str;
    enum error e = E_NONE;

    if (!valid(oid))
	e = E_INVARG;
    else if (!db_object_allows(oid, progr, FLAG_WRITE))
	e = E_PERM;
    else if (!db_delete_propdef(oid, pname))
	e = E_PROPNF;

    free_var(arglist);
    if (e == E_NONE)
	return no_var_pack();
    else
	return make_error_pack(e);
}

static package
bf_clear_prop(Var arglist, Byte next, void *vdata, Objid progr)
{				/* (object, prop-name) */
    Objid oid = arglist.v.list[1].v.obj;
    const char *pname = arglist.v.list[2].v.str;
    db_prop_handle h;
    Var value;
    enum error e;

    if (!valid(oid))
	e = E_INVARG;
    else {
	h = db_find_property(oid, pname, 0);
	if (!h.ptr)
	    e = E_PROPNF;
	else if (h.built_in
		 || (progr != db_property_owner(h) && !is_wizard(progr)))
	    e = E_PERM;
	else if (h.definer == oid)
	    e = E_INVARG;
	else {
	    value.type = TYPE_CLEAR;
	    db_set_property_value(h, value);
	    e = E_NONE;
	}
    }

    free_var(arglist);
    if (e == E_NONE)
	return no_var_pack();
    else
	return make_error_pack(e);
}

static package
bf_is_clear_prop(Var arglist, Byte next, void *vdata, Objid progr)
{				/* (object, prop-name) */
    Objid oid = arglist.v.list[1].v.obj;
    const char *pname = arglist.v.list[2].v.str;
    Var r;
    db_prop_handle h;
    enum error e;

    if (!valid(oid))
	e = E_INVARG;
    else {
	h = db_find_property(oid, pname, 0);
	if (!h.ptr)
	    e = E_PROPNF;
	else if (!h.built_in && !db_property_allows(h, progr, PF_READ))
	    e = E_PERM;
	else {
	    r.type = TYPE_INT;
	    r.v.num = (!h.built_in && db_property_value(h).type == TYPE_CLEAR);
	    e = E_NONE;
	}
    }

    free_var(arglist);
    if (e == E_NONE)
	return make_var_pack(r);
    else
	return make_error_pack(e);
}

void
register_property(void)
{
    (void) register_function("properties", 1, 1, bf_properties, TYPE_OBJ);
    (void) register_function("property_info", 2, 2, bf_prop_info,
			     TYPE_OBJ, TYPE_STR);
    (void) register_function("set_property_info", 3, 3, bf_set_prop_info,
			     TYPE_OBJ, TYPE_STR, TYPE_LIST);
    (void) register_function("add_property", 4, 4, bf_add_prop,
			     TYPE_OBJ, TYPE_STR, TYPE_ANY, TYPE_LIST);
    (void) register_function("delete_property", 2, 2, bf_delete_prop,
			     TYPE_OBJ, TYPE_STR);
    (void) register_function("clear_property", 2, 2, bf_clear_prop,
			     TYPE_OBJ, TYPE_STR);
    (void) register_function("is_clear_property", 2, 2, bf_is_clear_prop,
			     TYPE_OBJ, TYPE_STR);
}
