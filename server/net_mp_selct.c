/* Multiplexing wait implementation using the BSD UNIX select() system call */

#include <errno.h>		/* errno */
#include <string.h>		/* bzero() or memset(), used in FD_ZERO */
#include <sys/time.h>	/* select(), struct timeval */
#include <sys/types.h>		/* fd_set, FD_ZERO(), FD_SET(), FD_ISSET() */

#include "log.h"
#include "net_mplex.h"

static fd_set input, output;
static int max_descriptor;

void
mplex_clear(void)
{
    FD_ZERO(&input);
    FD_ZERO(&output);
    max_descriptor = -1;
}

void
mplex_add_reader(int fd)
{
    FD_SET(fd, &input);
    if (fd > max_descriptor)
	max_descriptor = fd;
}

void
mplex_add_writer(int fd)
{
    FD_SET(fd, &output);
    if (fd > max_descriptor)
	max_descriptor = fd;
}

int
mplex_wait(unsigned timeout)
{
    struct timeval tv;
    int n;

    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    n = select(max_descriptor + 1, (void *) &input, (void *) &output, 0, &tv);

    if (n < 0) {
	if (errno != EINTR)
	    log_perror("Waiting for network I/O");
	return 1;
    } else
	return (n == 0);
}

int
mplex_is_readable(int fd)
{
    return FD_ISSET(fd, &input);
}

int
mplex_is_writable(int fd)
{
    return FD_ISSET(fd, &output);
}
