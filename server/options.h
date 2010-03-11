#ifndef Options_h
#define Options_h 1

/******************************************************************************
 * The server is prepared to keep a log of every command entered by players
 * since the last checkpoint.  The log is flushed whenever a checkpoint is
 * successfully begun and is dumped into the server log when and if the server
 * panics.  Define LOG_COMMANDS to enable this logging.
 */

#define LOG_COMMANDS

/******************************************************************************
 * The server normally forks a separate process to make database checkpoints;
 * the original process continues to service user commands as usual while the
 * new process writes out the contents of its copy of memory to a disk file.
 * This checkpointing process can take quite a while, depending on how big your
 * database is, so it's usually quite convenient that the server can continue
 * to be responsive while this is taking place.  On some systems, however,
 * there may not be enough memory to support two simultaneously running server
 * processes.  Define UNFORKED_CHECKPOINTS to disable server forking for
 * checkpoints.
 */

#define UNFORKED_CHECKPOINTS

/******************************************************************************
 * If OUT_OF_BAND_PREFIX is defined as a non-empty string, then any lines of
 * input from any player that begin with that prefix will bypass both normal
 * command parsing and any pending read()ing task, instead spawning a server
 * task invoking #0:do_out_of_band_command(word-list).  This is intended for
 * use by fancy clients that need to send reliably-understood messages to the
 * server.
 */

#define OUT_OF_BAND_PREFIX "#$#"

/******************************************************************************
 * If OUT_OF_BAND_QUOTE_PREFIX is defined as a non-empty string, then any
 * lines of input from any player that begin with that prefix will be
 * stripped of that prefix and processed normally (whether to be parsed a
 * command or given to a pending read()ing task), even if the resulting line
 * begins with OUT_OF_BAND_PREFIX.  This provides a means of quoting lines
 * that would otherwise spawn #0:do_out_of_band_command tasks
 */

#define OUT_OF_BAND_QUOTE_PREFIX "#$\""

/******************************************************************************
 * The following constants define the execution limits placed on all MOO tasks.
 *
 * DEFAULT_MAX_STACK_DEPTH is the default maximum depth allowed for the MOO
 *	verb-call stack, the maximum number of dynamically-nested calls at any
 *	given time.  If defined in the database and larger than this default,
 *	$server_options.max_stack_depth overrides this default.
 * DEFAULT_FG_TICKS and DEFAULT_BG_TICKS are the default maximum numbers of
 *	`ticks' (basic operations) any task is allowed to use without
 *	suspending.  If defined in the database, $server_options.fg_ticks and
 *	$server_options.bg_ticks override these defaults.
 * DEFAULT_FG_SECONDS and DEFAULT_BG_SECONDS are the default maximum numbers of
 *	real-time seconds any task is allowed to use without suspending.  If
 *	defined in the database, $server_options.fg_seconds and
 *	$server_options.bg_seconds override these defaults.
 *
 * The *FG* constants are used only for `foreground' tasks (those started by
 * either player input or the server's initiative and that have never
 * suspended); the *BG* constants are used only for `background' tasks (forked
 * tasks and those of any kind that have suspended).
 *
 * The values given below are documented in the LambdaMOO Programmer's Manual,
 * so they should either be left as they are or else the manual should be
 * updated.
 */

#define DEFAULT_MAX_STACK_DEPTH	100

#define DEFAULT_FG_TICKS	300000
#define DEFAULT_BG_TICKS	150000

#define DEFAULT_FG_SECONDS	4
#define DEFAULT_BG_SECONDS	2

/******************************************************************************
 * Debug settings:
 *
 * DEBUG_LOG_TRACEBACKS prints all tracebacks to the server log.
 */

/* #define DEBUG_LOG_TRACEBACKS */

/******************************************************************************
 * NETWORK_PROTOCOL must be defined as one of the following:
 *
 * NP_SINGLE	The server will accept only one user at a time, communicating
 *		with them using the standard input and output streams of the
 *		server itself.
 * NP_TCP	The server will use TCP/IP protocols, such as are used by the
 *		Internet `telnet' command.
 * NP_LOCAL	The server will use UNIX-domain sockets.
 *
 * If NP_TCP is selected, then DEFAULT_PORT is the TCP port number on which the
 * server listens when no port argument is given on the command line.
 *
 * If NP_LOCAL is selected, then DEFAULT_CONNECT_FILE is the name of the UNIX
 * pseudo-file through which the server will listen for connections when no
 * file name is given on the command line.
 */

#define NETWORK_PROTOCOL 	NP_TCP
#define DEFAULT_PORT 		7777
#define DEFAULT_CONNECT_FILE	"/tmp/.MOO-server"

/******************************************************************************
 * The built-in MOO function open_network_connection(), when enabled,
 * allows (only) wizard-owned MOO code to make outbound network connections
 * from the server.  When disabled, it raises E_PERM whenever called.
 *
 * The +O and -O command line options can explicitly enable and disable this
 * function.  If neither option is supplied, the definition given to
 * OUTBOUND_NETWORK here determines the default behavior
 * (use 0 to disable by default, 1 or blank to enable by default).
 * 
 * If OUTBOUND_NETWORK is not defined at all,
 * open_network_connection() is permanently disabled and +O is ignored.
 *
 * *** THINK VERY HARD BEFORE ENABLING THIS FUNCTION ***
 * In some contexts, this could represent a serious breach of security.  
 *
 * Note: OUTBOUND_NETWORK may not be defined if NETWORK_PROTOCOL is either
 *	 NP_SINGLE or NP_LOCAL.
 */

/* disable by default, +O enables: */
/* #define OUTBOUND_NETWORK 0 */

/* enable by default, -O disables: */
#define OUTBOUND_NETWORK 1


/******************************************************************************
 * The following constants define certain aspects of the server's network
 * behavior if NETWORK_PROTOCOL is not defined as NP_SINGLE.
 *
 * MAX_QUEUED_OUTPUT is the maximum number of output characters the server is
 *		     willing to buffer for any given network connection before
 *		     discarding old output to make way for new.  The server
 *		     only discards output after attempting to send as much as
 *		     possible on the connection without blocking.
 * MAX_QUEUED_INPUT is the maximum number of input characters the server is
 *		    willing to buffer from any given network connection before
 *		    it stops reading from the connection at all.  The server
 *		    starts reading from the connection again once most of the
 *		    buffered input is consumed.
 * DEFAULT_CONNECT_TIMEOUT is the default number of seconds an un-logged-in
 *			   connection is allowed to remain idle without being
 *			   forcibly closed by the server; this can be
 *			   overridden by defining the `connect_timeout'
 *			   property on $server_options or on L, for connections
 *			   accepted by a given listener L.
 */

#define MAX_QUEUED_OUTPUT	1048576
#define MAX_QUEUED_INPUT	MAX_QUEUED_OUTPUT
#define DEFAULT_CONNECT_TIMEOUT	300

/******************************************************************************
 * On connections that have not been set to binary mode, the server normally
 * discards incoming characters that are not printable ASCII, including
 * backspace (8) and delete(127).  If INPUT_APPLY_BACKSPACE is defined,
 * backspace and delete cause the preceding character (if any) to be removed
 * from the input stream.  (Comment this out to restore pre-1.8.3 behavior)
 */
#define INPUT_APPLY_BACKSPACE

/******************************************************************************
 * The server maintains a cache of the most recently used patterns from calls
 * to the match() and rmatch() built-in functions.  PATTERN_CACHE_SIZE controls
 * how many past patterns are remembered by the server.  Do not set it to a
 * number less than 1.
 */

#define PATTERN_CACHE_SIZE	50

/******************************************************************************
 * If you don't plan on using protecting built-in properties (like
 * .name and .location), define IGNORE_PROP_PROTECTED.  The extra
 * property lookups on every reference to a built-in property are
 * expensive.
 ****************************************************************************** 
 */

#define IGNORE_PROP_PROTECTED

/******************************************************************************
 * The code generator can now recognize situations where the code will not
 * refer to the value of a variable again and generate opcodes that will
 * keep the interpreter from holding references to the value in the runtime
 * environment variable slot.  Before, when doing something like x=f(x), the
 * interpreter was guaranteed to have a reference to the value of x while f()
 * was running, meaning that f() always had to copy x to modify it.  With
 * BYTECODE_REDUCE_REF enabled, f() could be called with the last reference
 * to the value of x.  So for example, x={@x,y} can (if there are no other
 * references to the value of x in variables or properties) just append to
 * x rather than make a copy and append to that.  If it *does* have to copy,
 * the next time (if it's in a loop) it will have the only reference to the
 * copy and then it can take advantage.
 *
 * NOTE WELL    NOTE WELL    NOTE WELL    NOTE WELL    NOTE WELL    
 *
 * This option affects the length of certain bytecode sequences.
 * Suspended tasks in a database from a server built with this option
 * are not guaranteed to work with a server built without this option,
 * and vice versa.  It is safe to flip this switch only if there are
 * no suspended tasks in the database you are loading.  (It might work
 * anyway, but hey, it's your database.)  This restriction will be
 * lifted in a future version of the server software.  Consider this
 * option as being BETA QUALITY until then.
 *
 * NOTE WELL    NOTE WELL    NOTE WELL    NOTE WELL    NOTE WELL    
 *
 ******************************************************************************
 */
/* #define BYTECODE_REDUCE_REF */

#ifdef BYTECODE_REDUCE_REF
#error Think carefully before enabling BYTECODE_REDUCE_REF.  This feature is still beta.  Comment out this line if you are sure.
#endif

/******************************************************************************
 * The server can merge duplicate strings on load to conserve memory.  This
 * involves a rather expensive step at startup to dispose of the table used
 * to find the duplicates.  This should be improved eventually, but you may
 * want to trade off faster startup time for increased memory usage.
 *
 * You might want to turn this off if you see a large delay before the
 * INTERN: lines in the log at startup.
 ******************************************************************************
 */

#define STRING_INTERNING /* */

/******************************************************************************
 * Store the length of the string WITH the string rather than recomputing
 * it each time it is needed.
 ******************************************************************************
 */
/* #define MEMO_STRLEN */

/******************************************************************************
 * Turn on WAIF_DICT for Jay Carlson's patch that makes waif[x]=y and waif[x]
 * work by calling verbs on the waif.
 ******************************************************************************
 */
#define WAIF_DICT

/*****************************************************************************
 ********** You shouldn't need to change anything below this point. **********
 *****************************************************************************/

#ifndef OUT_OF_BAND_PREFIX
#define OUT_OF_BAND_PREFIX ""
#endif
#ifndef OUT_OF_BAND_QUOTE_PREFIX
#define OUT_OF_BAND_QUOTE_PREFIX ""
#endif

#if PATTERN_CACHE_SIZE < 1
#  error Illegal match() pattern cache size!
#endif

#define NETWORK_STYLE NS_BSD
#define MPLEX_STYLE MP_SELECT

#define NP_SINGLE	1
#define NP_TCP		2
#define NP_LOCAL	3

#define NS_BSD		1

#define MP_SELECT	1

#include "config.h"


#if (NETWORK_PROTOCOL == NP_LOCAL || NETWORK_PROTOCOL == NP_SINGLE) && defined(OUTBOUND_NETWORK)
#  error You cannot define "OUTBOUND_NETWORK" with that "NETWORK_PROTOCOL"
#endif

/* make sure OUTBOUND_NETWORK has a value;
   for backward compatibility, use 1 if none given */
#if defined(OUTBOUND_NETWORK) && (( 0 * OUTBOUND_NETWORK - 1 ) == 0)
#undef OUTBOUND_NETWORK
#define OUTBOUND_NETWORK 1
#endif


#if NETWORK_PROTOCOL != NP_LOCAL && NETWORK_PROTOCOL != NP_SINGLE && NETWORK_PROTOCOL != NP_TCP
#  error Illegal value for "NETWORK_PROTOCOL"
#endif

#if NETWORK_STYLE != NS_BSD
#  error Illegal value for "NETWORK_STYLE"
#endif

#endif				/* !Options_h */
