/* Copyright 1989 Digital Equipment Corporation.                             */
/* Distributed only by permission.                                           */
/*****************************************************************************/
/* File: exceptions.c                                                        */
/* Taken originally from:                                                    */
/*              Implementing Exceptions in C                                 */
/*              Eric S. Roberts                                              */
/*              Research Report #40                                          */
/*              DEC Systems Research Center                                  */
/*              March 21, 1989                                               */
/* Modified slightly by Pavel Curtis for use in the LambdaMOO server code.   */
/* ------------------------------------------------------------------------- */
/* Implementation of the C exception handler.  Most of the real work is      */
/* done in the macros defined in the exceptions.h header file.               */
/*****************************************************************************/

#include <setjmp.h>

#include "exceptions.h"

ES_CtxBlock *ES_exceptionStack = 0;

Exception ANY;

void
ES_RaiseException(Exception * exception, int value)
{
    ES_CtxBlock *cb, *xb;
    int i;

    for (xb = ES_exceptionStack; xb; xb = xb->link) {
	for (i = 0; i < xb->nx; i++) {
	    if (xb->array[i] == exception || xb->array[i] == &ANY)
		goto doneSearching;
	}
    }

  doneSearching:
    if (!xb)
	panic("Unhandled exception!");

    for (cb = ES_exceptionStack; cb != xb && !cb->finally; cb = cb->link);
    ES_exceptionStack = cb;
    cb->id = exception;
    cb->value = value;
    longjmp((void *) cb->jmp, 1);
}
