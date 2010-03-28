#include <float.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "log.h"
#include "storage.h"
#include "streams.h"
#include "utf.h"

Stream *
new_stream(int size)
{
    Stream *s = (Stream *)mymalloc(sizeof(Stream), M_STREAM);

    if (size < 1)
	size = 1;

    s->buffer = (char *)mymalloc(size, M_STREAM);
    s->buflen = size;
    s->current = 0;

    return s;
}

static void
grow(Stream * s, int newlen)
{
    char *newbuf;

    newbuf = (char *)mymalloc(newlen, M_STREAM);
    memcpy(newbuf, s->buffer, s->current);
    myfree(s->buffer, M_STREAM);
    s->buffer = newbuf;
    s->buflen = newlen;
}

void
stream_add_char(Stream * s, char c)
{
    if (s->current + 1 >= s->buflen)
	grow(s, s->buflen * 2);

    s->buffer[s->current++] = c;
}

int
stream_add_utf(Stream * s, int c)
{
    int result;

    if (s->current + 4 >= s->buflen)
	grow(s, s->buflen * 2 + 4);

    char *b = s->buffer + s->current;
    result = put_utf(&b, c);
    s->current = b - s->buffer;

    return result;
}

void
stream_delete_char(Stream * s)
{
    if (s->current > 0)
      s->current--;
}

void
stream_delete_utf(Stream * s)
{
    if (s->current > 0) {
        s->current--;
        while ((s->buffer[s->current] & 0xc0) == 0x80) {
            s->current--;
        }
    }
}

void
stream_add_string(Stream * s, const char *string)
{
    int len = strlen(string);

    if (s->current + len >= s->buflen) {
	int newlen = s->buflen * 2;

	if (newlen <= s->current + len)
	    newlen = s->current + len + 1;
	grow(s, newlen);
    }
    strcpy(s->buffer + s->current, string);
    s->current += len;
}

void
stream_add_bytes(Stream * s, const char *bytes, int len)
{
    if (s->current + len >= s->buflen) {
	int newlen = s->buflen * 2;

	if (newlen <= s->current + len)
	    newlen = s->current + len + 1;
	grow(s, newlen);
    }
    memcpy(s->buffer + s->current, bytes, len);
    s->current += len;
}

void
stream_printf(Stream * s, const char *fmt,...)
{
    va_list args, pargs;
    int len;

    va_start(args, fmt);

    va_copy(pargs, args);
    len = vsnprintf(s->buffer + s->current, s->buflen - s->current, fmt, pargs);
    va_end(pargs);

    if (s->current + len >= s->buflen) {
	int newlen = s->buflen * 2;

	if (newlen <= s->current + len)
	    newlen = s->current + len + 1;
	grow(s, newlen);
	len = vsnprintf(s->buffer + s->current, s->buflen - s->current, fmt, args);
    }
    va_end(args);
    s->current += len;
}

void
free_stream(Stream * s)
{
    myfree(s->buffer, M_STREAM);
    myfree(s, M_STREAM);
}

char *
reset_stream(Stream * s)
{
    s->buffer[s->current] = '\0';
    s->current = 0;
    return s->buffer;
}

char *
stream_contents(Stream * s)
{
    s->buffer[s->current] = '\0';
    return s->buffer;
}

int
stream_length(Stream * s)
{
    return s->current;
}
