#include "config.h"

#if 0
extern void addref(const void *p);
extern unsigned int delref(const void *p);
#else
#define addref(X) (++((int *)(X))[-1])
#define delref(X) (--((int *)(X))[-1])
#define refcount(X) (((int *)(X))[-1])
#endif
