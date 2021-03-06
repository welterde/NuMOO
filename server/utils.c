#include "utf-ctype.h"
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "db.h"
#include "db_io.h"
#include "exceptions.h"
#include "list.h"
#include "log.h"
#include "match.h"
#include "numbers.h"
#include "ref_count.h"
#include "server.h"
#include "storage.h"
#include "streams.h"
#include "structures.h"
#include "utf.h"
#include "waif.h"
#include "utils.h"

/*
 * These versions of strcasecmp() and strncasecmp() depend on ASCII.
 * We implement them here because neither one is in the ANSI standard.
 */

static const char cmap[] =
"\000\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017"
"\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037"
"\040\041\042\043\044\045\046\047\050\051\052\053\054\055\056\057"
"\060\061\062\063\064\065\066\067\070\071\072\073\074\075\076\077"
"\100\141\142\143\144\145\146\147\150\151\152\153\154\155\156\157"
"\160\161\162\163\164\165\166\167\170\171\172\133\134\135\136\137"
"\140\141\142\143\144\145\146\147\150\151\152\153\154\155\156\157"
"\160\161\162\163\164\165\166\167\170\171\172\173\174\175\176\177"
"\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217"
"\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237"
"\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257"
"\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277"
"\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317"
"\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337"
"\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357"
"\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377";

int
mystrcasecmp(const char *ss, const char *tt)
{
    register const unsigned char *s = (const unsigned char *) ss;
    register const unsigned char *t = (const unsigned char *) tt;

    if (s == t) {
	return 0;
    }
    while (cmap[*s] == cmap[*t++]) {
	if (!*s++)
	    return 0;
    }
    return (cmap[*s] - cmap[*--t]);
}

int
mystrncasecmp(const char *ss, const char *tt, int n)
{
    const unsigned char *s = (const unsigned char *) ss;
    const unsigned char *t = (const unsigned char *) tt;

    if (!n || ss == tt)
	return 0;
    while (cmap[*s] == cmap[*t++]) {
	if (!*s++ || !--n)
	    return 0;
    }
    return (cmap[*s] - cmap[*--t]);
}

int
verbcasecmp(const char *verb, const char *word)
{
    const unsigned char *w;
    const unsigned char *v = (const unsigned char *) verb;
    enum {
	none, inner, end
    } star;

    if (verb == word) {
	return 1;
    }
    while (*v) {
	w = (const unsigned char *) word;
	star = none;
	while (1) {
	    while (*v == '*') {
		v++;
		star = (!*v || *v == ' ') ? end : inner;
	    }
	    if (!*v || *v == ' ' || !*w || cmap[*w] != cmap[*v])
		break;
	    w++;
	    v++;
	}
	if (!*w ? (star != none || !*v || *v == ' ')
	    : (star == end))
	    return 1;
	while (*v && *v != ' ')
	    v++;
	while (*v == ' ')
	    v++;
    }
    return 0;
}

unsigned
str_hash(const char *s)
{
    register unsigned ans = 0;

    while (*s) {
	ans = (ans << 3) + (ans >> 28) + cmap[(unsigned char) *s++];
    }
    return ans;
}

void
complex_free_var(Var v)
{
    int i;

    switch (v.type) {
    case TYPE_STR:
	if (v.v.str)
	    free_str(v.v.str);
	break;
    case TYPE_LIST:
	if (delref(v.v.list) == 0) {
	    Var *pv;

	    for (i = v.v.list[0].v.num, pv = v.v.list + 1; i > 0; i--, pv++)
		free_var(*pv);
	    myfree(v.v.list, M_LIST);
	}
	break;
    default:
	break;
    case TYPE_WAIF:
	if (delref(v.v.waif) == 0)
	    free_waif(v.v.waif);
	break;
    }
}

Var
complex_var_ref(Var v)
{
    switch (v.type) {
    case TYPE_STR:
	addref(v.v.str);
	break;
    case TYPE_LIST:
	addref(v.v.list);
	break;
    default:
	break;
    case TYPE_WAIF:
	addref(v.v.waif);
	break;
    }
    return v;
}

Var
complex_var_dup(Var v)
{
    int i;
    Var newlist;

    switch (v.type) {
    case TYPE_STR:
	v.v.str = str_dup(v.v.str);
	break;
    case TYPE_LIST:
	newlist = new_list(v.v.list[0].v.num);
	for (i = 1; i <= v.v.list[0].v.num; i++) {
	    newlist.v.list[i] = var_ref(v.v.list[i]);
	}
	v.v.list = newlist.v.list;
	break;
    default:
	break;
    case TYPE_WAIF:
	v.v.waif = dup_waif(v.v.waif);
    }
    return v;
}

/* could be inlined and use complex_etc like the others, but this should
 * usually be called in a context where we already konw the type.
 */
int
var_refcount(Var v)
{
    switch (v.type) {
    case TYPE_STR:
	return refcount(v.v.str);
	break;
    case TYPE_LIST:
	return refcount(v.v.list);
	break;
    default:
	break;
    }
    return 1;
}

int
is_true(Var v)
{
    return ((v.type == TYPE_INT && v.v.num != 0)
	    || (v.type == TYPE_FLOAT && v.v.fnum != 0.0)
	    || (v.type == TYPE_STR && v.v.str && *v.v.str != '\0')
	    || (v.type == TYPE_LIST && v.v.list[0].v.num != 0));
}

int
equality(Var lhs, Var rhs, int case_matters)
{
    if (lhs.type == TYPE_FLOAT || rhs.type == TYPE_FLOAT)
	return do_equals(lhs, rhs);
    else if (lhs.type != rhs.type)
	return 0;
    else {
	switch (lhs.type) {
	case TYPE_INT:
	    return lhs.v.num == rhs.v.num;
	case TYPE_OBJ:
	    return lhs.v.obj == rhs.v.obj;
	case TYPE_ERR:
	    return lhs.v.err == rhs.v.err;
	case TYPE_STR:
	    if (case_matters)
		return !strcmp(lhs.v.str, rhs.v.str);
	    else
		return !mystrcasecmp(lhs.v.str, rhs.v.str);
	case TYPE_LIST:
	    if (lhs.v.list[0].v.num != rhs.v.list[0].v.num)
		return 0;
	    else {
		int i;

		if (lhs.v.list == rhs.v.list) {
		    return 1;
		}
		for (i = 1; i <= lhs.v.list[0].v.num; i++) {
		    if (!equality(lhs.v.list[i], rhs.v.list[i], case_matters))
			return 0;
		}
		return 1;
	    }
	case TYPE_WAIF:
	    /* compare them or assert same-waif? */
	    return lhs.v.waif == rhs.v.waif;
	default:
	    panic("EQUALITY: Unknown value type");
	}
    }
    return 0;
}

char *
strsub(const char *source, const char *what, const char *with, int case_counts)
{
    static Stream *str = 0;
    int lwhat = strlen(what);

    if (str == 0)
	str = new_stream(100);

    while (*source) {
	if (!(case_counts ? strncmp(source, what, lwhat)
	      : mystrncasecmp(source, what, lwhat))) {
	    stream_add_string(str, with);
	    source += lwhat;
	} else
	    stream_add_char(str, *source++);
    }

    return reset_stream(str);
}

int
strindex(const char *source, const char *what, int case_counts)
{
    const char *s, *e;
    int lwhat = strlen(what);
    int ind = 0;

    for (s = source, e = source + strlen(source) - lwhat; s <= e; get_utf(&s), ind++) {
	if (!(case_counts ? strncmp(s, what, lwhat)
	      : mystrncasecmp(s, what, lwhat))) {
	    return ind + 1;
	}
    }
    return 0;
}

int
strrindex(const char *source, const char *what, int case_counts)
{
    const char *s;
    int lwhat = strlen(what);
    int ind = 0;
    s = source + strlen(source) - lwhat;
    if (s < source)
        return 0;
    while ((*s & 0xc0) == 0x80) {
        /* UTF8 continuation -- nothing can start here */
        s--;
    }
    ind = strlen_utf(source) - strlen_utf(s);

    for (; s >= source; s--, ind--) {
	if (!(case_counts ? strncmp(s, what, lwhat)
	      : mystrncasecmp(s, what, lwhat))) {
	    return ind + 1;
	}
        while ((*s & 0xc0) == 0x80) {
            /* UTF8 continuation */ 
            s--;
        }
    }
    return 0;
}

Var
get_system_property(const char *name)
{
    Var value;
    db_prop_handle h;

    if (!valid(SYSTEM_OBJECT)) {
	value.type = TYPE_ERR;
	value.v.err = E_INVIND;
	return value;
    }
    h = db_find_property(SYSTEM_OBJECT, name, &value);
    if (!h.ptr) {
	value.type = TYPE_ERR;
	value.v.err = E_PROPNF;
    } else if (!h.built_in)	/* make two cases the same */
	value = var_ref(value);
    return value;
}

Objid
get_system_object(const char *name)
{
    Var value;

    value = get_system_property(name);
    if (value.type != TYPE_OBJ) {
	free_var(value);
	return NOTHING;
    } else
	return value.v.obj;
}

int
value_bytes(Var v)
{
    int i, len, size = sizeof(Var);

    switch (v.type) {
    case TYPE_STR:
	size += memo_strlen(v.v.str) + 1;
	break;
    case TYPE_FLOAT:
	size += sizeof(double);
	break;
    case TYPE_LIST:
	len = v.v.list[0].v.num;
	size += sizeof(Var);	/* for the `length' element */
	for (i = 1; i <= len; i++)
	    size += value_bytes(v.v.list[i]);
	break;
    case TYPE_WAIF:
	size += waif_bytes(v.v.waif);
	break;
    default:
	break;
    }

    return size;
}

const char *
raw_bytes_to_binary(const char *buffer, int buflen)
{
    static Stream *s = 0;
    int i;

    if (!s)
	s = new_stream(100);

    for (i = 0; i < buflen; i++) {
	unsigned char c = buffer[i];

	if (c >= 32 && c < 126)
	    stream_add_char(s, c);
	else
	    stream_printf(s, "~%02x", (int) c);
    }

    return reset_stream(s);
}

const char *
binary_to_raw_bytes(const char *binary, int *buflen)
{
    static Stream *s = 0;
    const char *ptr = binary;

    if (!s)
	s = new_stream(100);
    else
	reset_stream(s);

    while (*ptr) {
	unsigned char c = *ptr++;

	if (c != '~')
	    stream_add_char(s, c);
	else {
	    int i;
	    char cc = 0;

	    for (i = 1; i <= 2; i++) {
		/* The | 0x20 changes upper case to lower case for the
		   characters we are interested in here... */
		c = *ptr++ | 0x20;
		if ('0' <= c && c <= '9')
		    cc = (cc << 4) + (c - '0');
		else if ('a' <= c && c <= 'f')
		    cc = (cc << 4) + (c - 'a' + 10);
		else
		    return 0;
	    }

	    stream_add_char(s, cc);
	}
    }

    *buflen = stream_length(s);
    return reset_stream(s);
}

const char *
string_quote(const char *si)
{
  Stream *s = new_stream(strlen(si) + 5);
  const unsigned char *str = (const unsigned char*)si;
	
  stream_add_char(s, '"');
  while (*str) {
    switch (*str) {
    case '\a':	stream_add_string(s, "\\a"); str++; break;
    case '\b':	stream_add_string(s, "\\b"); str++; break;
    case '\e':	stream_add_string(s, "\\e"); str++; break;
    case '\f':	stream_add_string(s, "\\f"); str++; break;
    case '\n':	stream_add_string(s, "\\n"); str++; break;
    case '\r':	stream_add_string(s, "\\r"); str++; break;
    case '\t':	stream_add_string(s, "\\t"); str++; break;
    case '\v':	stream_add_string(s, "\\v"); str++; break;

    case '"':	case '\\':
      stream_add_char(s, '\\');
      /* fall thru */
    default:
	stream_add_char(s, *str++);
    }
  }
  stream_add_char(s, '"');
  si = str_dup(reset_stream(s));
  free_stream(s);
  return si;
}
