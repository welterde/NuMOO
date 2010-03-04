/* BSD/LOCAL MUD client */

#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"
#include "options.h"

void
main(int argc, char **argv)
{
    const char *connect_file = DEFAULT_CONNECT_FILE;
    int s;
    struct sockaddr_un address;

    if (argc == 2)
	connect_file = argv[1];
    else if (argc != 1) {
	fprintf(stderr, "Usage: %s [server-connect-file]\n", argv[0]);
	exit(1);
    }
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
	perror("socket");
	exit(1);
    }
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, connect_file);

    if (connect(s, (struct sockaddr *) &address,
		sizeof(address.sun_family) + strlen(connect_file)) < 0) {
	perror("connect");
	exit(1);
    }
    while (1) {
	fd_set input;
	char buffer[1024];

	FD_ZERO(&input);
	FD_SET(0, &input);
	FD_SET(s, &input);

	if (select(s + 1, (void *) &input, 0, 0, 0) < 0) {
	    if (errno != EINTR) {
		perror("select");
		exit(1);
	    }
	} else {
	    if (FD_ISSET(0, &input))
		write(s, buffer, read(0, buffer, sizeof(buffer)));
	    if (FD_ISSET(s, &input)) {
		int count = read(s, buffer, sizeof(buffer));

		if (count == 0)
		    break;
		write(1, buffer, count);
	    }
	}
    }

    exit(0);
}
