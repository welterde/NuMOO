#include "config.h"
#include "execute.h"
#include "structures.h"

extern vm read_vm(int task_id);
extern void write_vm(vm);

extern vm new_vm(int task_id, int stack_size);
extern void free_vm(vm the_vm, int stack_too);

extern activation top_activ(vm);
extern Objid progr_of_cur_verb(vm);
extern unsigned suspended_lineno_of_vm(vm);
