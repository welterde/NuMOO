#include "options.h"

#  if NETWORK_PROTOCOL == NP_TCP
#    if NETWORK_STYLE == NS_BSD
#      include "net_bsd_tcp.c"
#    endif
#  endif

#  if NETWORK_PROTOCOL == NP_LOCAL
#    if NETWORK_STYLE == NS_BSD
#      include "net_bsd_lcl.c"
#    endif
#  endif
