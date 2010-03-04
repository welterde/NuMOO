/* This describes the complete set of procedures that a multiplexing wait
 * implementation must provide.
 *
 * The `mplex' abstraction provides a way to wait until it is possible to
 * perform an I/O operation on any of a set of file descriptors without
 * blocking.  Uses of the abstraction always have the following form:
 *
 *      mplex_clear();
 *      { mplex_add_reader(fd)  or  mplex_add_writer(fd) }*
 *      timed_out = mplex_wait(timeout);
 *      { mplex_is_readable(fd)  or  mplex_is_writable(fd) }*
 *
 * The set of file descriptors maintained by the abstraction is referred to
 * below as the `wait set'.  Each file descriptor in the wait set is marked
 * with the kind of I/O (i.e., reading, writing, or both) desired.
 */

#ifndef Net_MPlex_H
#define Net_MPlex_H 1

extern void mplex_clear(void);
				/* Reset the wait set to be empty. */

extern void mplex_add_reader(int fd);
				/* Add the given file descriptor to the wait
				 * set, marked for reading.
				 */

extern void mplex_add_writer(int fd);
				/* Add the given file descriptor to the wait
				 * set, marked for writing.
				 */

extern int mplex_wait(unsigned timeout);
				/* Wait until it is possible either to do the
				 * appropriate kind of I/O on some descriptor
				 * in the wait set or until `timeout' seconds
				 * have elapsed.  Return true iff the timeout
				 * expired without any I/O becoming possible.
				 */

extern int mplex_is_readable(int fd);
				/* Return true iff the most recent mplex_wait()
				 * call terminated (in part) because reading
				 * had become possible on the given descriptor.
				 */

extern int mplex_is_writable(int fd);
				/* Return true iff the most recent mplex_wait()
				 * call terminated (in part) because reading
				 * had become possible on the given descriptor.
				 */

#endif				/* !Net_MPlex_H */
