/*
 *  The server_version is a character string containing three decimal numbers
 *  separated by periods:
 *
 *      <major>.<minor>.<release>
 *
 *  The major version number changes very slowly, only when existing MOO code
 *  might stop working, due to an incompatible change in the syntax or
 *  semantics of the programming language, or when an incompatible change is
 *  made to the database format.
 *
 *  The minor version number changes more quickly, whenever an upward-
 *  compatible change is made in the programming language syntax or semantics.
 *  The most common cause of this is the addition of a new kind of expression,
 *  statement, or built-in function.
 *
 *  The release version number changes as frequently as bugs are fixed in the
 *  server code.  Changes in the release number indicate changes that should
 *  only be visible to users as bug fixes, if at all.
 *
 */

#include "config.h"
#include "version.h"

const char *server_version = "1.8.3";

int
check_version(DB_Version version)
{
    return version < Num_DB_Versions;
}
