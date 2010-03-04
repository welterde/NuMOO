#include "structures.h"

#include <float.h>
#include <math.h>
#include <limits.h>

#define IS_REAL(x)	(-HUGE_VAL < (x) && (x) < HUGE_VAL)

#ifndef DECIMAL_DIG
# define DECIMAL_DIG (DBL_DIG+4)
#endif


extern Var new_float(double);
extern enum error become_integer(Var, Num *, int);

extern int do_equals(Var, Var);
extern int compare_integers(Num, Num);
extern Var compare_numbers(Var, Var);

extern Var do_add(Var, Var);
extern Var do_subtract(Var, Var);
extern Var do_multiply(Var, Var);
extern Var do_divide(Var, Var);
extern Var do_modulus(Var, Var);
extern Var do_power(Var, Var);
