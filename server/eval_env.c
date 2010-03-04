#include "config.h"
#include "eval_env.h"
#include "storage.h"
#include "structures.h"
#include "sym_table.h"
#include "utils.h"

/*
 * Keep a pool of rt_envs big enough to hold NUM_READY_VARS variables to
 * avoid lots of malloc/free.
 */
static Var *ready_size_rt_envs;

Var *
new_rt_env(unsigned size)
{
    Var *ret;
    unsigned i;

    if (size <= NUM_READY_VARS && ready_size_rt_envs) {
	ret = ready_size_rt_envs;
	ready_size_rt_envs = ret[0].v.list;
    } else
	ret = mymalloc(MAX(size, NUM_READY_VARS) * sizeof(Var), M_RT_ENV);

    for (i = 0; i < size; i++)
	ret[i].type = TYPE_NONE;

    return ret;
}

void
free_rt_env(Var * rt_env, unsigned size)
{
    register unsigned i;

    for (i = 0; i < size; i++)
	free_var(rt_env[i]);

    if (size <= NUM_READY_VARS) {
	rt_env[0].v.list = ready_size_rt_envs;
	ready_size_rt_envs = rt_env;
    } else
	myfree((void *) rt_env, M_RT_ENV);
}

Var *
copy_rt_env(Var * from, unsigned size)
{
    unsigned i;

    Var *ret = new_rt_env(size);
    for (i = 0; i < size; i++)
	ret[i] = var_ref(from[i]);
    return ret;
}

void
fill_in_rt_consts(Var * env, DB_Version version)
{
    Var v;

    v.type = TYPE_INT;
    v.v.num = (int) TYPE_ERR;
    env[SLOT_ERR] = var_ref(v);
    v.v.num = (int) TYPE_INT;
    env[SLOT_NUM] = var_ref(v);
    v.v.num = (int) _TYPE_STR;
    env[SLOT_STR] = var_ref(v);
    v.v.num = (int) TYPE_OBJ;
    env[SLOT_OBJ] = var_ref(v);
    v.v.num = (int) _TYPE_LIST;
    env[SLOT_LIST] = var_ref(v);

    if (version >= DBV_Float) {
	v.v.num = (int) TYPE_INT;
	env[SLOT_INT] = var_ref(v);
	v.v.num = (int) _TYPE_FLOAT;
	env[SLOT_FLOAT] = var_ref(v);
    }
}

void
set_rt_env_obj(Var * env, int slot, Objid o)
{
    Var v;
    v.type = TYPE_OBJ;
    v.v.obj = o;
    env[slot] = var_ref(v);
}

void
set_rt_env_str(Var * env, int slot, const char *s)
{
    Var v;
    v.type = TYPE_STR;
    v.v.str = s;
    env[slot] = v;
}

void
set_rt_env_var(Var * env, int slot, Var v)
{
    env[slot] = v;
}
