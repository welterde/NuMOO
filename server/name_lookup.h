/*
 * This module provides IP host name lookup with timeouts.  Because
 * many DNS servers are flaky and the normal UNIX name-lookup facilities just
 * hang in such situations, this interface comes in very handy.
 */

#ifndef Name_Lookup_H
#define Name_Lookup_H 1

#include "config.h"

extern int initialize_name_lookup(void);
				/* Initialize the module, returning true iff
				 * this succeeds.
				 */

extern uint32_t lookup_addr_from_name(const char *name,
				      unsigned timeout);
				/* Translate a host name to a 32-bit
				 * internet address in host byte order.  If
				 * anything goes wrong, return 0.  Dotted
				 * decimal address are translated properly.
				 */

extern const char *lookup_name_from_addr(struct sockaddr_in *addr,
					 unsigned timeout);
				/* Translate an internet address, contained
				 * in the sockaddr_in, to a host name.  If
				 * the translation cannot be done, the
				 * address is returned in dotted decimal
				 * form.
				 */

#endif				/* Name_Lookup_H */
