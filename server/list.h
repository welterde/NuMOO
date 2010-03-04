#include "structures.h"

extern Var listappend(Var list, Var value);
extern Var listinsert(Var list, Var value, int pos);
extern Var listdelete(Var list, int pos);
extern Var listset(Var list, Var value, int pos);
extern Var listrangeset(Var list, int from, int to, Var value);
extern Var listconcat(Var first, Var second);
extern int ismember(Var value, Var list, int case_matters);
extern Var setadd(Var list, Var value);
extern Var setremove(Var list, Var value);
extern Var sublist(Var list, int lower, int upper);
extern Var strrangeset(Var list, int from, int to, Var value);
extern Var substr(Var str, int lower, int upper);
extern Var strget(Var str, Var i);
extern Var new_list(int size);
extern const char *value2str(Var);
extern const char *value_to_literal(Var);
