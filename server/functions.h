#ifndef Functions_h
#define Functions_h 1

#include <stdio.h>

#include "config.h"
#include "execute.h"
#include "program.h"
#include "structures.h"

typedef struct {
    enum {
	BI_RETURN,		/* Normal function return */
	BI_RAISE,		/* Raising an error */
	BI_CALL,		/* Making a nested verb call */
	BI_SUSPEND,		/* Suspending the current task */
	BI_KILL			/* Kill the current task */
    } kind;
    union {
	Var ret;
	struct {
	    Var code;
	    const char *msg;
	    Var value;
	} raise;
	struct {
	    Byte pc;
	    void *data;
	} call;
	struct {
	    enum error (*proc) (vm, void *);
	    void *data;
	} susp;
    } u;
} package;

void register_bi_functions(void);

package make_kill_pack(void);
package make_error_pack(enum error err);
package make_raise_pack(enum error err, const char *msg, Var value);
package make_var_pack(Var v);
package make_int_pack(Num v);
package make_float_pack(double v);
package no_var_pack(void);
package make_call_pack(Byte pc, void *data);
package tail_call_pack(void);
package make_suspend_pack(enum error (*)(vm, void *), void *);

typedef package(*bf_type) (Var, Byte, void *, Objid);
typedef void (*bf_write_type) (void *vdata);
typedef void *(*bf_read_type) (void);

#define MAX_FUNC         256
#define FUNC_NOT_FOUND   MAX_FUNC
/* valid function numbers are 0 - 255, or a total of 256 of them.
   function number 256 is reserved for func_not_found signal.
   hence valid function numbers will fit in one byte but the 
   func_not_found signal will not */

extern const char *name_func_by_num(unsigned);
extern unsigned number_func_by_name(const char *);

extern unsigned register_function(const char *, int, int, bf_type,...);
extern unsigned register_function_with_read_write(const char *, int, int,
						  bf_type, bf_read_type,
						  bf_write_type,...);

extern package call_bi_func(unsigned, Var, Byte, Objid, void *);
/* will free or use Var arglist */

extern void write_bi_func_data(void *vdata, Byte f_id);
extern int read_bi_func_data(Byte f_id, void **bi_func_state,
			     Byte * bi_func_pc);
extern Byte *pc_for_bi_func_data(void);

extern void load_server_options(void);

#endif
