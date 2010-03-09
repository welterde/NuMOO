/*****************************************************************************
 * Routines for use by non-DB modules with persistent state stored in the DB
 *****************************************************************************/

#include "config.h"
#include <ctype.h>
#include <float.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db_io.h"
#include "db_private.h"
#include "exceptions.h"
#include "list.h"
#include "log.h"
#include "numbers.h"
#include "parser.h"
#include "storage.h"
#include "streams.h"
#include "structures.h"
#include "str_intern.h"
#include "unparse.h"
#include "version.h"
#include "waif.h"


/*********** Input ***********/

static FILE *input;

void
dbpriv_set_dbio_input(FILE * f)
{
    input = f;
}


/* This function is used for reading string data from the database
* Anything that is stored as a string in the database has a special format.
* so that \n can be used in strings, in the file,  \n will be replaced
* with the two-character identifier "\0n"  The string representation in
* memory will remain unchanged (normal string thingie)
*
* Therefore, you can't use fgets here, and we must implement it ourselves
*/

int
dbio_read_line(char *s, int n)
{
    int pos, c;
    
    for (pos = 0; pos < (n - 1); pos++)
	{
            c = fgetc(input);  /* read next char from file */
            switch (c)
		{
		case '\n':
                    /* We have reached the end of the record, add the \0 and return */
                    s[pos] = '\n';
                    s[pos+1] = '\0';
                    return 0;
                    break;
		case EOF:
                    if (!pos) return -1;
                    s[pos] = '\0';
                    return 0;
                    break;
		case '\0':
                    /* Handle the special lil escaping thing */
                    c = fgetc(input);
                    switch (c)
			{
			case 'n':
                            s[pos] = '\n';
                            break;
			case 'r':
                            s[pos] = '\r';
                            break;
			default:
                            errlog("DBIO_READ_LINE: Unknown escape character in string at file pos. %ld\n",
                                   ftell(input));
                            return -1;
			}
                    break;
		default:
                    s[pos] = c;
		}
	}
    return 1;
}


int
dbio_scanf(const char *format,...)
{
    va_list args;
    int count;
    const char *ptr;

    va_start(args, format);
    count = vfscanf(input, format, args);
    va_end(args);

    return count;
}

int64_t
dbio_read_num(void)
{
    char s[22];
    char *p;
    long long i;

    fgets(s, sizeof(s), input);
    i = strtoll(s, &p, 10);
    if (isspace(*s) || *p != '\n')
	errlog("DBIO_READ_NUM: Bad number: \"%s\" at file pos. %ld\n",
	       s, ftell(input));
    return i;
}

double
dbio_read_float(void)
{
    char s[40];
    char *p;
    double d;

    fgets(s, 40, input);
    d = strtod(s, &p);
    if (isspace(*s) || *p != '\n')
	errlog("DBIO_READ_FLOAT: Bad number: \"%s\" at file pos. %ld\n",
	       s, ftell(input));
    return d;
}

Objid
dbio_read_objid(void)
{
    return dbio_read_num();
}

static long int prior_to_read_string;

const char *
dbio_read_string(void)
{
    static Stream *str = 0;
    static char buffer[1024];
    int len,status=0,used_stream = 0;
    
    prior_to_read_string = ftell(input);
    
    if (str == 0)
	str = new_stream(1024);
    
 try_again:
    status = dbio_read_line(buffer, sizeof(buffer));
    len = strlen(buffer);
    if (len == sizeof(buffer) - 1 && status > 0) {
	stream_add_string(str, buffer);
	used_stream = 1;
	goto try_again;
    }
    if (buffer[len - 1] == '\n')
	buffer[len - 1] = '\0';
    
    if (used_stream) {
	stream_add_string(str, buffer);
	return reset_stream(str);
    } else
	return buffer;
}

const char *
dbio_read_string_intern(void)
{
    const char *s, *r;

    s = dbio_read_string();
    r = str_intern(s);

    /* puts(r); */

    return r;
}


Var
dbio_read_var(void)
{
    Var r;
    int i, l = dbio_read_num();

    if (l == (int) TYPE_ANY && dbio_input_version == DBV_Prehistory)
	l = TYPE_NONE;		/* Old encoding for VM's empty temp register
				 * and any as-yet unassigned variables.
				 */
    r.type = (var_type) l;
    switch (l) {
    case TYPE_CLEAR:
    case TYPE_NONE:
	break;
    case _TYPE_STR:
	r.v.str = dbio_read_string_intern();
	r.type |= TYPE_COMPLEX_FLAG;
	break;
    case TYPE_OBJ:
    case TYPE_ERR:
    case TYPE_INT:
    case TYPE_CATCH:
    case TYPE_FINALLY:
	r.v.num = dbio_read_num();
	break;
    case _TYPE_FLOAT:
	r.v.fnum = dbio_read_float();
	break;
    case _TYPE_LIST:
	l = dbio_read_num();
	r = new_list(l);
	for (i = 0; i < l; i++)
	    r.v.list[i + 1] = dbio_read_var();
	break;
    case _TYPE_WAIF:
	r = read_waif();
	break;
    default:
	errlog("DBIO_READ_VAR: Unknown type (%d) at DB file pos. %ld\n",
	       l, ftell(input));
	r = zero;
	break;
    }
    return r;
}

struct state {
    char prev_char;
    const char *(*fmtr) (void *);
    void *data;
};

static const char *
program_name(struct state *s)
{
    if (!s->fmtr)
	return s->data;
    else
	return (*s->fmtr) (s->data);
}

static void
my_error(void *data, const char *msg)
{
    errlog("PARSER: Error in %s:\n", program_name(data));
    errlog("           %s\n", msg);
}

static void
my_warning(void *data, const char *msg)
{
    oklog("PARSER: Warning in %s:\n", program_name(data));
    oklog("           %s\n", msg);
}

static int
my_getc(void *data)
{
    struct state *s = data;
    int c;

    c = fgetc(input);
    if (c == '.' && s->prev_char == '\n') {
	/* end-of-verb marker in DB */
	c = fgetc(input);	/* skip next newline */
	return EOF;
    }
    if (c == EOF)
	my_error(data, "Unexpected EOF");
    s->prev_char = c;
    return c;
}

static Parser_Client parser_client =
{my_error, my_warning, my_getc};

Program *
dbio_read_program(DB_Version version, const char *(*fmtr) (void *), void *data)
{
    struct state s;

    s.prev_char = '\n';
    s.fmtr = fmtr;
    s.data = data;
    return parse_program(version, parser_client, &s);
}


/*********** Output ***********/

Exception dbpriv_dbio_failed;

static FILE *output;

void
dbpriv_set_dbio_output(FILE * f)
{
    output = f;
}

void
dbio_printf(const char *format,...)
{
    va_list args;

    va_start(args, format);
    if (vfprintf(output, format, args) < 0)
	RAISE(dbpriv_dbio_failed, 0);
    va_end(args);
}

void
dbio_write_num(int64_t n)
{
    dbio_printf("%"PRId64"\n", n);
}

void
dbio_write_float(double d)
{
    static const char *fmt = 0;
    static char buffer[10];

    if (!fmt) {
	sprintf(buffer, "%%.%dg\n", DBL_DIG + 4);
	fmt = buffer;
    }
    dbio_printf(fmt, d);
}

void
dbio_write_objid(Objid oid)
{
    dbio_write_num(oid);
}


void
dbio_write_string(const char *s)
{
    int pos;
    
    /* Handle null strings */
    if (!s) {
        fputc('\n', output);
        return;
    }
    
    /* Iterate over each character, printing to the file and Escaping as nessecary. */
    for (pos = 0; s[pos]; pos++)
	{
            switch (s[pos])
		{
                case '\n':
                    fputc('\0', output);
                    fputc('n', output);
                    break;
                case '\r':
                    fputc('\0', output);
                    fputc('r', output);
                    break;
                default:
                    fputc(s[pos], output);
		}
	}
    fputc('\n', output);
    
    /* dbio_printf("%s\n", s ? s : ""); */
}

void
dbio_write_var(Var v)
{
    int i;

    dbio_write_num((int) v.type & TYPE_DB_MASK);
    switch ((int) v.type) {
    case TYPE_CLEAR:
    case TYPE_NONE:
	break;
    case TYPE_STR:
	dbio_write_string(v.v.str);
	break;
    case TYPE_OBJ:
    case TYPE_ERR:
    case TYPE_INT:
    case TYPE_CATCH:
    case TYPE_FINALLY:
	dbio_write_num(v.v.num);
	break;
    case TYPE_FLOAT:
	dbio_write_float(v.v.fnum);
	break;
    case TYPE_LIST:
	dbio_write_num(v.v.list[0].v.num);
	for (i = 0; i < v.v.list[0].v.num; i++)
	    dbio_write_var(v.v.list[i + 1]);
	break;
    case TYPE_WAIF:
	write_waif(v);
	break;
    }
}

static void
receiver(void *data, const char *line)
{
    dbio_printf("%s\n", line);
}

void
dbio_write_program(Program * program)
{
    unparse_program(program, receiver, 0, 1, 0, MAIN_VECTOR);
    dbio_printf(".\n");
}

void
dbio_write_forked_program(Program * program, int f_index)
{
    unparse_program(program, receiver, 0, 1, 0, f_index);
    dbio_printf(".\n");
}
