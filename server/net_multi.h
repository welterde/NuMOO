#ifndef Net_Multi_H
#define Net_Multi_H 1

/* Extra networking facilities available only for the multi-user networking
 * configurations.
 */

typedef void (*network_fd_callback) (int fd, void *data);

extern void network_register_fd(int fd, network_fd_callback readable,
				network_fd_callback writable, void *data);
				/* The file descriptor FD will be selected for
				 * at intervals (whenever the networking module
				 * is doing its own I/O processing).  If FD
				 * selects true for reading and READABLE is
				 * non-zero, then READABLE will be called,
				 * passing FD and DATA.  Similarly for
				 * WRITABLE.
				 */

extern void network_unregister_fd(int fd);
				/* Any existing registration for FD is
				 * forgotten.
				 */

extern int network_set_nonblocking(int fd);
				/* Enable nonblocking I/O on the file
				 * descriptor FD.  Return true iff successful.
				 */

#endif				/* !Net_Multi_H */
