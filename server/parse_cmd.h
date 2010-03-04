#ifndef Parse_Cmd_H
#define Parse_Cmd_H 1

#include "config.h"
#include "db.h"
#include "structures.h"

typedef struct {
    const char *verb;		/* verb (as typed by player) */
    const char *argstr;		/* arguments to verb */
    Var args;			/* arguments to the verb */

    const char *dobjstr;	/* direct object string */
    Objid dobj;			/* direct object */

    const char *prepstr;	/* preposition string */
    db_prep_spec prep;		/* preposition identifier */

    const char *iobjstr;	/* indirect object string */
    Objid iobj;			/* indirect object */
} Parsed_Command;

extern char **parse_into_words(char *input, int *nwords);
extern Var parse_into_wordlist(const char *command);
extern Parsed_Command *parse_command(const char *command, Objid user);
extern void free_parsed_command(Parsed_Command *);

#endif				/* !Parse_Cmd_H */
