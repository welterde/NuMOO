#include <string.h>

#include "ast.h"
#include "config.h"
#include "list.h"
#include "log.h"
#include "program.h"
#include "structures.h"
#include "sym_table.h"
#include "storage.h"
#include "utils.h"

struct entry {
    void *ptr;
    Memory_Type type;
};

static int pool_size, next_pool_slot;
static struct entry *pool;

void
begin_code_allocation()
{
    pool_size = 10;
    next_pool_slot = 0;
    pool = (struct entry *)mymalloc(pool_size * sizeof(struct entry), M_AST_POOL);
}

void
end_code_allocation(int aborted)
{
    if (aborted) {
	int i;

	for (i = 0; i < next_pool_slot; i++) {
	    if (pool[i].ptr != 0)
		myfree(pool[i].ptr, pool[i].type);
	}
    }
    myfree(pool, M_AST_POOL);
}

static void *
allocate(int size, Memory_Type type)
{
    if (next_pool_slot >= pool_size) {	/* enlarge the pool */
	struct entry *new_pool;
	int i;

	pool_size *= 2;
	new_pool = (struct entry *)mymalloc(pool_size * sizeof(struct entry), M_AST_POOL);
	for (i = 0; i < next_pool_slot; i++) {
	    new_pool[i] = pool[i];
	}
	myfree(pool, M_AST_POOL);
	pool = new_pool;
    }
    pool[next_pool_slot].type = type;
    return pool[next_pool_slot++].ptr = mymalloc(size, type);
}

static void
deallocate(void *ptr)
{
    int i;

    for (i = 0; i < next_pool_slot; i++) {
	if (ptr == pool[i].ptr) {
	    myfree(ptr, pool[i].type);
	    pool[i].ptr = 0;
	    return;
	}
    }

    errlog("DEALLOCATE: Unknown pointer deallocated\n");
}

char *
alloc_string(const char *buffer)
{
    char *string = allocate(strlen(buffer) + 1, M_STRING);

    strcpy(string, buffer);
    return string;
}

void
dealloc_string(char *str)
{
    deallocate(str);
}

double *
alloc_float(double value)
{
    double *d = allocate(sizeof(double), M_FLOAT);

    *d = value;
    return d;
}

void
dealloc_node(void *node)
{
    deallocate(node);
}

Stmt *
alloc_stmt(enum Stmt_Kind kind)
{
    Stmt *result = allocate(sizeof(Stmt), M_AST);

    result->kind = kind;
    result->next = 0;
    return result;
}

Cond_Arm *
alloc_cond_arm(Expr * condition, Stmt * stmt)
{
    Cond_Arm *result = allocate(sizeof(Cond_Arm), M_AST);

    result->condition = condition;
    result->stmt = stmt;
    result->next = 0;
    return result;
}

Except_Arm *
alloc_except(int id, Arg_List * codes, Stmt * stmt)
{
    Except_Arm *result = allocate(sizeof(Except_Arm), M_AST);

    result->id = id;
    result->codes = codes;
    result->stmt = stmt;
    result->label = 0;
    result->next = 0;
    return result;
}

Expr *
alloc_expr(enum Expr_Kind kind)
{
    Expr *result = allocate(sizeof(Expr), M_AST);

    result->kind = kind;
    return result;
}

Expr *
alloc_var(var_type type)
{
    Expr *result = alloc_expr(EXPR_VAR);

    result->e.var.type = type;
    return result;
}

Expr *
alloc_binary(enum Expr_Kind kind, Expr * lhs, Expr * rhs)
{
    Expr *result = alloc_expr(kind);

    result->e.bin.lhs = lhs;
    result->e.bin.rhs = rhs;
    return result;
}

Expr *
alloc_verb(Expr * obj, Expr * verb, Arg_List * args)
{
    Expr *result = alloc_expr(EXPR_VERB);

    result->e.verb.obj = obj;
    result->e.verb.verb = verb;
    result->e.verb.args = args;
    return result;
}

Arg_List *
alloc_arg_list(enum Arg_Kind kind, Expr * expr)
{
    Arg_List *result = allocate(sizeof(Arg_List), M_AST);

    result->kind = kind;
    result->expr = expr;
    result->next = 0;
    return result;
}

Scatter *
alloc_scatter(enum Scatter_Kind kind, int id, Expr * expr)
{
    Scatter *sc = allocate(sizeof(Scatter), M_AST);

    sc->kind = kind;
    sc->id = id;
    sc->expr = expr;
    sc->next = 0;
    sc->label = sc->next_label = 0;

    return sc;
}

static void free_expr(Expr *);

static void
free_arg_list(Arg_List * args)
{
    Arg_List *arg, *next_arg;

    for (arg = args; arg; arg = next_arg) {
	next_arg = arg->next;
	free_expr(arg->expr);
	myfree(arg, M_AST);
    }
}

static void
free_scatter(Scatter * sc)
{
    Scatter *next_sc;

    for (; sc; sc = next_sc) {
	next_sc = sc->next;
	if (sc->expr)
	    free_expr(sc->expr);
	myfree(sc, M_AST);
    }
}

static void
free_expr(Expr * expr)
{
    switch (expr->kind) {

    case EXPR_VAR:
	free_var(expr->e.var);
	break;

    case EXPR_ID:
    case EXPR_LENGTH:
	/* Do nothing. */
	break;

    case EXPR_PROP:
    case EXPR_INDEX:
    case EXPR_PLUS:
    case EXPR_MINUS:
    case EXPR_TIMES:
    case EXPR_DIVIDE:
    case EXPR_MOD:
    case EXPR_AND:
    case EXPR_OR:
    case EXPR_EQ:
    case EXPR_NE:
    case EXPR_LT:
    case EXPR_LE:
    case EXPR_GT:
    case EXPR_GE:
    case EXPR_IN:
    case EXPR_ASGN:
    case EXPR_EXP:
	free_expr(expr->e.bin.lhs);
	free_expr(expr->e.bin.rhs);
	break;

    case EXPR_COND:
	free_expr(expr->e.cond.condition);
	free_expr(expr->e.cond.consequent);
	free_expr(expr->e.cond.alternate);
	break;

    case EXPR_VERB:
	free_expr(expr->e.verb.obj);
	free_expr(expr->e.verb.verb);
	free_arg_list(expr->e.verb.args);
	break;

    case EXPR_RANGE:
	free_expr(expr->e.range.base);
	free_expr(expr->e.range.from);
	free_expr(expr->e.range.to);
	break;

    case EXPR_CALL:
	free_arg_list(expr->e.call.args);
	break;

    case EXPR_NEGATE:
    case EXPR_NOT:
	free_expr(expr->e.expr);
	break;

    case EXPR_LIST:
	free_arg_list(expr->e.list);
	break;

    case EXPR_CATCH:
	free_expr(expr->e.catchexp.tryexp);
	free_arg_list(expr->e.catchexp.codes);
	if (expr->e.catchexp.except)
	    free_expr(expr->e.catchexp.except);
	break;

    case EXPR_SCATTER:
	free_scatter(expr->e.scatter);
	break;

    default:
	errlog("FREE_EXPR: Unknown Expr_Kind: %d\n", expr->kind);
	break;
    }

    myfree(expr, M_AST);
}

void
free_stmt(Stmt * stmt)
{
    Stmt *next_stmt;
    Cond_Arm *arm, *next_arm;
    Except_Arm *except, *next_e;

    for (; stmt; stmt = next_stmt) {
	next_stmt = stmt->next;

	switch (stmt->kind) {

	case STMT_COND:
	    for (arm = stmt->s.cond.arms; arm; arm = next_arm) {
		next_arm = arm->next;
		free_expr(arm->condition);
		free_stmt(arm->stmt);
		myfree(arm, M_AST);
	    }
	    if (stmt->s.cond.otherwise)
		free_stmt(stmt->s.cond.otherwise);
	    break;

	case STMT_LIST:
	    free_expr(stmt->s.list.expr);
	    free_stmt(stmt->s.list.body);
	    break;

	case STMT_RANGE:
	    free_expr(stmt->s.range.from);
	    free_expr(stmt->s.range.to);
	    free_stmt(stmt->s.range.body);
	    break;

	case STMT_WHILE:
	    free_expr(stmt->s.loop.condition);
	    free_stmt(stmt->s.loop.body);
	    break;

	case STMT_FORK:
	    free_expr(stmt->s.fork.time);
	    free_stmt(stmt->s.fork.body);
	    break;

	case STMT_EXPR:
	case STMT_RETURN:
	    if (stmt->s.expr)
		free_expr(stmt->s.expr);
	    break;

	case STMT_TRY_EXCEPT:
	    free_stmt(stmt->s.catchexp.body);
	    for (except = stmt->s.catchexp.excepts; except; except = next_e) {
		next_e = except->next;
		free_arg_list(except->codes);
		free_stmt(except->stmt);
		myfree(except, M_AST);
	    }
	    break;

	case STMT_TRY_FINALLY:
	    free_stmt(stmt->s.finally.body);
	    free_stmt(stmt->s.finally.handler);
	    break;

	case STMT_BREAK:
	case STMT_CONTINUE:
	    break;		/* Nothing extra to free */

	default:
	    errlog("FREE_STMT: unknown Stmt_Kind: %d\n", stmt->kind);
	    break;
	}

	myfree(stmt, M_AST);
    }
}
