/* C code produced by gperf version 2.1p1 (K&R C version, modified by Pavel) */
/* Command-line: pgperf -aCIptT -k1,3,$ keywords.gperf  */

#include "utf-ctype.h"

	/* -*- C -*- */

#include <string.h>

#include "config.h"
#include "keywords.h"
#include "tokens.h"
#include "utils.h"


#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 9
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 106
/*
   35 keywords
   104 is the maximum key range
 */

static int
hash(register const char *str, register int len)
{
    static const unsigned char hash_table[] =
    {
	106, 106, 106, 106, 106, 106, 106, 106, 106, 106,
	106, 106, 106, 106, 106, 106, 106, 106, 106, 106,
	106, 106, 106, 106, 106, 106, 106, 106, 106, 106,
	106, 106, 106, 106, 106, 106, 106, 106, 106, 106,
	106, 106, 106, 106, 106, 106, 106, 106, 106, 106,
	106, 106, 106, 106, 106, 106, 106, 106, 106, 106,
	106, 106, 106, 106, 106, 106, 106, 106, 106, 106,
	106, 106, 106, 106, 106, 106, 106, 106, 106, 106,
	106, 106, 106, 106, 106, 106, 106, 106, 106, 106,
	106, 106, 106, 106, 106, 106, 106, 10, 0, 45,
	0, 0, 0, 10, 106, 45, 106, 10, 106, 35,
	5, 106, 5, 10, 0, 25, 55, 106, 35, 5,
	106, 10, 106, 106, 106, 106, 106, 106,
    };
    register int hval = len;

    switch (hval) {
    default:
    case 3:
	hval += hash_table[tolower((unsigned char) str[2])];
    case 2:
    case 1:
	hval += hash_table[tolower((unsigned char) str[0])];
    }
    return hval + hash_table[tolower((unsigned char) str[len - 1])];
}

static int
case_strcmp(register const char *str, register const char *key)
{
    int ans = 0;

    while (!(ans = tolower(*str) - (int) *key) && *str)
	str++, key++;

    return ans;
}

const struct keyword *
in_word_set(register const char *str, register int len)
{

    static const struct keyword wordlist[] =
    {
	{"",},
	{"",},
	{"",},
	{"for", DBV_Prehistory, tFOR},
	{"",},
	{"endif", DBV_Prehistory, tENDIF},
	{"endfor", DBV_Prehistory, tENDFOR},
	{"e_range", DBV_Prehistory, tERROR, E_RANGE},
	{"endwhile", DBV_Prehistory, tENDWHILE},
	{"e_recmove", DBV_Prehistory, tERROR, E_RECMOVE},
	{"",},
	{"e_none", DBV_Prehistory, tERROR, E_NONE},
	{"",},
	{"e_propnf", DBV_Prehistory, tERROR, E_PROPNF},
	{"fork", DBV_Prehistory, tFORK},
	{"break", DBV_BreakCont, tBREAK},
	{"endtry", DBV_Exceptions, tENDTRY},
	{"endfork", DBV_Prehistory, tENDFORK},
	{"",},
	{"",},
	{"",},
	{"",},
	{"finally", DBV_Exceptions, tFINALLY},
	{"",},
	{"",},
	{"",},
	{"",},
	{"e_quota", DBV_Prehistory, tERROR, E_QUOTA},
	{"",},
	{"else", DBV_Prehistory, tELSE},
	{"",},
	{"elseif", DBV_Prehistory, tELSEIF},
	{"",},
	{"any", DBV_Exceptions, tANY},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"e_div", DBV_Prehistory, tERROR, E_DIV},
	{"e_args", DBV_Prehistory, tERROR, E_ARGS},
	{"e_varnf", DBV_Prehistory, tERROR, E_VARNF},
	{"e_verbnf", DBV_Prehistory, tERROR, E_VERBNF},
	{"",},
	{"",},
	{"e_perm", DBV_Prehistory, tERROR, E_PERM},
	{"if", DBV_Prehistory, tIF},
	{"",},
	{"",},
	{"",},
	{"",},
	{"in", DBV_Prehistory, tIN},
	{"e_invind", DBV_Prehistory, tERROR, E_INVIND},
	{"",},
	{"while", DBV_Prehistory, tWHILE},
	{"e_nacc", DBV_Prehistory, tERROR, E_NACC},
	{"",},
	{"continue", DBV_BreakCont, tCONTINUE},
	{"",},
	{"",},
	{"e_type", DBV_Prehistory, tERROR, E_TYPE},
	{"e_float", DBV_Float, tERROR, E_FLOAT},
	{"e_invarg", DBV_Prehistory, tERROR, E_INVARG},
	{"",},
	{"",},
	{"return", DBV_Prehistory, tRETURN},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"try", DBV_Exceptions, tTRY},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"e_maxrec", DBV_Prehistory, tERROR, E_MAXREC},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"",},
	{"except", DBV_Exceptions, tEXCEPT},
    };

    if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
	register int key = hash(str, len);

	if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE) {
	    register const char *s = wordlist[key].name;

	    if (*s == tolower(*str) && !case_strcmp(str + 1, s + 1))
		return &wordlist[key];
	}
    }
    return 0;
}

const struct keyword *
find_keyword(const char *word)
{
    return in_word_set(word, strlen(word));
}
