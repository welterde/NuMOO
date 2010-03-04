#ifndef Stream_h
#define Stream_h 1

#include "config.h"

typedef struct {
    char *buffer;
    int buflen;
    int current;
} Stream;

extern Stream *new_stream(int size);
extern void stream_add_char(Stream *, char);
extern int stream_add_utf(Stream *, int);
extern void stream_delete_char(Stream *);
extern void stream_delete_utf(Stream *);
extern void stream_add_string(Stream *, const char *);
extern void stream_add_bytes(Stream *, const char *, int);
extern void stream_printf(Stream *, const char *,...) FORMAT(printf,2,3);
extern void free_stream(Stream *);
extern char *stream_contents(Stream *);
extern char *reset_stream(Stream *);
extern int stream_length(Stream *);

#endif
