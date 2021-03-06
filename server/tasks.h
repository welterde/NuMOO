#ifndef Tasks_H
#define Tasks_H 1

#include "config.h"
#include "execute.h"
#include "structures.h"

typedef struct {
    void *ptr;
} task_queue;

extern task_queue new_task_queue(Objid player, Objid handler);
extern void free_task_queue(task_queue q);

extern int tasks_connection_option(task_queue, const char *,
				   Var *);
extern Var tasks_connection_options(task_queue, Var);
extern int tasks_set_connection_option(task_queue, const char *,
				       Var);

extern void new_input_task(task_queue, const char *, int);
extern void task_suspend_input(task_queue);
extern enum error enqueue_forked_task2(activation a, int f_index,
                                       double after_seconds, int vid);
extern enum error enqueue_suspended_task(vm the_vm, void *data);
				/* data == &(double after_seconds) */
extern enum error make_reading_task(vm the_vm, void *data);
				/* data == &(Objid connection) */
extern void resume_task(vm the_vm, Var value);
				/* Make THE_VM (a suspended task) runnable on
				 * the appropriate task queue; when it resumes
				 * execution, return VALUE from the built-in
				 * function that suspended it.  If VALUE.type
				 * is TYPE_ERR, then VALUE is raised instead of
				 * returned.
				 */
extern vm find_suspended_task(int id);

/* External task queues:

 * The registered enumerator should apply the given closure to every VM in the
 * external queue for as long as the closure returns TEA_CONTINUE, also passing
 * it a short string describing the current state of the VM (relative to that
 * queue) and the (void *) datum passed to the enumerator.  If the closure
 * returns TEA_KILL, then the task associated with that VM has been killed and
 * the VM freed; the external queue should clean up any local state associated
 * with the queue.  If the closure returns TEA_STOP or TEA_KILL, the enumerator
 * should immediately return without applying the closure to any further VMs.
 * The enumerator should return the same value as was returned by the last
 * invocation of the closure, or TEA_CONTINUE if there were no VMs in the
 * queue.
 */
typedef enum {
    TEA_CONTINUE,		/* Enumerator should continue with next VM */
    TEA_STOP,			/* Enumerator should stop now */
    TEA_KILL			/* Enumerator should stop and forget VM */
} task_enum_action;

typedef task_enum_action(*task_closure) (vm, const char *, void *);
typedef task_enum_action(*task_enumerator) (task_closure, void *);
extern void register_task_queue(task_enumerator);

extern Var read_input_now(Objid connection);

extern int next_task_start(void);
extern void run_ready_tasks(void);
extern enum outcome run_server_task(Objid player, Objid what,
				    const char *verb, Var args,
				    const char *argstr, Var * result);
extern enum outcome run_server_task_setting_id(Objid player, Objid what,
					       const char *verb, Var args,
					       const char *argstr,
					     Var * result, int *task_id);
extern enum outcome run_server_program_task(Objid thisobj, const char *verb,
					    Var args, Objid vloc,
					    const char *verbname,
					  Program * program, Objid progr,
					    int debug, Objid player,
					    const char *argstr,
					    Var * result);

extern int current_task_id;
extern int last_input_task_id(Objid player);

extern void write_task_queue(void);
extern int read_task_queue(void);

extern db_verb_handle find_verb_for_programming(Objid player,
						const char *verbref,
						const char **message,
						const char **vname);

#endif				/* !Tasks_H */
