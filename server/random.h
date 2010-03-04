#include "config.h"

#if HAVE_LRAND48
extern long lrand48(void);
extern void srand48(long);
#    define RANDOM	lrand48
#    define SRANDOM	srand48
#else
#  include <stdlib.h>
#  if HAVE_RANDOM
#    define RANDOM	random
#    define SRANDOM 	srandom
#  else
#    define RANDOM	rand
#    define SRANDOM	srand
#  endif
#endif
