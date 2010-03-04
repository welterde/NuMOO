#include "config.h"
#include "structures.h"
#include "version.h"

struct keyword {
    const char *name;		/* the canonical spelling of the keyword */
    DB_Version version;		/* the DB version when it was introduced */
    int token;			/* the type of token the scanner should use */
    enum error error;		/* for token == ERROR, the value */
};

typedef const struct keyword Keyword;

extern Keyword *find_keyword(const char *);
