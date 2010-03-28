#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "bf_register.h"
#include "config.h"
#include "functions.h"
#include "log.h"
#include "options.h"
#include "storage.h"
#include "streams.h"
#include "utils.h"

static FILE *log_file = 0;

void
set_log_file(FILE * f)
{
    log_file = f;
}

int log_pcount = 5000;
static time_t log_prev = 0;
int log_report_progress_cktime()
{
    time_t now = time(0);
    log_pcount = 5000;
    return ((now >= log_prev + 2) && (log_prev = now, 1));
}

static void
do_log(const char *prefix, const char *fmt, va_list args)
{
    FILE *f;

    log_prev = time(0);
    log_pcount = 5000;
    if (log_file) {
	char *nowstr = ctime(&log_prev);

	nowstr[19] = '\0';	/* kill the year and newline at the end */
	f = log_file;
	fprintf(f, "%s: %s", nowstr + 4, prefix);	/* skip the day of week */
    } else
	f = stderr;

    vfprintf(f, fmt, args);
    fflush(f);
}

void
oklog(const char *fmt,...)
{
    va_list args;

    va_start(args, fmt);
    do_log("", fmt, args);
    va_end(args);
}

void
errlog(const char *fmt,...)
{
    va_list args;

    va_start(args, fmt);
    do_log("*** ", fmt, args);
    va_end(args);
}

void
log_perror(const char *what)
{
    errlog("%s: %s\n", what, strerror(errno));
}


#ifdef LOG_COMMANDS
static Stream *command_history = 0;
#endif

void
reset_command_history()
{
#ifdef LOG_COMMANDS
    if (command_history == 0)
	command_history = new_stream(1024);
    else
	reset_stream(command_history);
#endif
}

void
log_command_history()
{
#ifdef LOG_COMMANDS
    errlog("COMMAND HISTORY:\n%s", stream_contents(command_history));
#endif
}

void
add_command_to_history(Objid player, const char *command)
{
#ifdef LOG_COMMANDS
    time_t now = time(0);
    char *nowstr = ctime(&now);

    nowstr[19] = '\0';		/* kill the year and newline at the end */
    stream_printf(command_history, "%s: #%"PRIdN": %s\n",
		  nowstr + 4,	/* skip day of week */
		  player, command);
#endif				/* LOG_COMMANDS */
}

/**** built in functions ****/

static package
bf_server_log(Var arglist, Byte next, void *vdata, Objid progr)
{
    if (!is_wizard(progr)) {
	free_var(arglist);
	return make_error_pack(E_PERM);
    } else {
	int is_error = (arglist.v.list[0].v.num == 2
			&& is_true(arglist.v.list[2]));

	if (is_error)
	    errlog("> %s\n", arglist.v.list[1].v.str);
	else
	    oklog("> %s\n", arglist.v.list[1].v.str);

	free_var(arglist);
	return no_var_pack();
    }
}

void
register_log(void)
{
    register_function("server_log", 1, 2, bf_server_log, TYPE_STR, TYPE_ANY);
}
