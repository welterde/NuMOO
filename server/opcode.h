#ifndef Opcode_h
#define Opcode_h 1

#include "options.h"

#define NUM_READY_VARS 32

enum Extended_Opcode {
    EOP_RANGESET, EOP_LENGTH,
    EOP_PUSH_LABEL, EOP_END_CATCH, EOP_END_EXCEPT, EOP_END_FINALLY,
    EOP_CONTINUE,

    /* ops after this point cost one tick */
    EOP_CATCH, EOP_TRY_EXCEPT, EOP_TRY_FINALLY,
    EOP_WHILE_ID, EOP_EXIT, EOP_EXIT_ID,
    EOP_SCATTER, EOP_EXP,

    Last_Extended_Opcode = 255
};

enum Opcode {

    /* control/statement constructs with 1 tick: */
    OP_IF, OP_WHILE, OP_EIF, OP_FORK, OP_FORK_WITH_ID, OP_FOR_LIST,
    OP_FOR_RANGE,

    /* expr-related opcodes with 1 tick: */
    OP_INDEXSET, OP_PUSH_GET_PROP, OP_GET_PROP, OP_CALL_VERB, OP_PUT_PROP,
    OP_BI_FUNC_CALL, OP_IF_QUES, OP_REF, OP_RANGE_REF,

    /* arglist-related opcodes with 1 tick: */
    OP_MAKE_SINGLETON_LIST, OP_CHECK_LIST_FOR_SPLICE,

    /* arith binary ops -- 1 tick: */
    OP_MULT, OP_DIV, OP_MOD, OP_ADD, OP_MINUS,

    /* comparison binary ops -- 1 tick: */
    OP_EQ, OP_NE, OP_LT, OP_LE, OP_GT, OP_GE, OP_IN,

    /* logic binary ops -- 1 tick: */
    OP_AND, OP_OR,

    /* unary ops -- 1 tick: */
    OP_UNARY_MINUS, OP_NOT,

    /* assignments, 1 tick: */
    OP_PUT,
    OP_G_PUT = OP_PUT + NUM_READY_VARS,

    /* variable references, no tick: */
    OP_PUSH,
    OP_G_PUSH = OP_PUSH + NUM_READY_VARS,

#ifdef BYTECODE_REDUCE_REF
    /* final variable references, no tick: */
    OP_PUSH_CLEAR,
    OP_G_PUSH_CLEAR = OP_PUSH_CLEAR + NUM_READY_VARS,
#endif				/* BYTECODE_REDUCE_REF */

    /* expr-related opcodes with no tick: */
    OP_IMM, OP_MAKE_EMPTY_LIST, OP_LIST_ADD_TAIL, OP_LIST_APPEND,
    OP_PUSH_REF, OP_PUT_TEMP, OP_PUSH_TEMP,

    /* control/statement constructs with no ticks: */
    OP_JUMP, OP_RETURN, OP_RETURN0, OP_DONE, OP_POP,

    OP_EXTENDED,		/* Used to add more opcodes */

    OPTIM_NUM_START,
    /* storage optimized imm-numbers can occupy 113-255, for 143 of them */
    Last_Opcode = 255
};

#define OPTIM_NUM_LOW -10
#define OPTIM_NUM_HI  (Last_Opcode - OPTIM_NUM_START + OPTIM_NUM_LOW)

#define IS_PUSH_n(o)             ((o) >= (unsigned) OP_PUSH \
				  && (o) < (unsigned) OP_G_PUSH)
#ifdef BYTECODE_REDUCE_REF
#define IS_PUSH_CLEAR_n(o)             ((o) >= (unsigned) OP_PUSH_CLEAR \
				  && (o) < (unsigned) OP_G_PUSH_CLEAR)
#define PUSH_CLEAR_n_INDEX(o)          ((o) - OP_PUSH_CLEAR)
#endif				/* BYTECODE_REDUCE_REF */
#define IS_PUT_n(o)              ((o) >= (unsigned) OP_PUT \
				  && (o) < (unsigned) OP_G_PUT)
#define PUSH_n_INDEX(o)          ((o) - OP_PUSH)
#define PUT_n_INDEX(o)           ((o) - OP_PUT)

#define IS_OPTIM_NUM_OPCODE(o)   ((o) >= (unsigned) OPTIM_NUM_START)
#define OPCODE_TO_OPTIM_NUM(o)   ((Num)(unsigned)(o) - OPTIM_NUM_START + OPTIM_NUM_LOW)

#define OPTIM_NUM_TO_OPCODE(i)   (OPTIM_NUM_START + (i) - OPTIM_NUM_LOW)
#define IN_OPTIM_NUM_RANGE(i)    ((i) >= OPTIM_NUM_LOW && (i) <= OPTIM_NUM_HI)

/* ARITH_COMP_BIN_OP does not include AND, OR */
#define IS_ARITH_COMP_BIN_OP(o)  ((o) >= (unsigned) OP_MULT \
				  && (o) <= (unsigned) OP_IN)

/* whether the opcode needs one tick */
#define COUNT_TICK(o)      	 ((o) <= OP_G_PUT)
#define COUNT_EOP_TICK(eo)	 ((eo) >= EOP_CATCH)

typedef enum Opcode Opcode;
typedef enum Extended_Opcode Extended_Opcode;

#endif
