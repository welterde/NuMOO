#ifndef Eval_env_h
#define Eval_env_h 1

#include "config.h"
#include "structures.h"
#include "version.h"

extern Var *new_rt_env(unsigned size);
extern void free_rt_env(Var * rt_env, unsigned size);
extern Var *copy_rt_env(Var * from, unsigned size);

void set_rt_env_obj(Var * env, int slot, Objid o);
void set_rt_env_str(Var * env, int slot, const char *s);
void set_rt_env_var(Var * env, int slot, Var v);

void fill_in_rt_consts(Var * env, DB_Version);

#endif
