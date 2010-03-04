#include "config.h"
#include "db.h"
#include "quota.h"
#include "structures.h"

static const char *quota_name = "ownership_quota";

int
decr_quota(Objid player)
{
    db_prop_handle h;
    Var v;

    if (!valid(player))
	return 1;

    h = db_find_property(player, quota_name, &v);
    if (!h.ptr)
	return 1;

    if (v.type != TYPE_INT)
	return 1;

    if (v.v.num <= 0)
	return 0;

    v.v.num--;
    db_set_property_value(h, v);
    return 1;
}

void
incr_quota(Objid player)
{
    db_prop_handle h;
    Var v;

    if (!valid(player))
	return;

    h = db_find_property(player, quota_name, &v);
    if (!h.ptr)
	return;

    if (v.type != TYPE_INT)
	return;

    v.v.num++;
    db_set_property_value(h, v);
}
