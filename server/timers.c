#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "config.h"
#include "timers.h"

typedef struct Timer_Entry Timer_Entry;
struct Timer_Entry {
    Timer_Entry *next;
    time_t when;
    Timer_Proc proc;
    Timer_Data data;
    Timer_ID id;
};

static Timer_Entry *active_timers = 0;
static Timer_Entry *free_timers = 0;
static Timer_Entry *virtual_timer = 0;
static Timer_ID next_id = 0;

static Timer_Entry *
allocate_timer(void)
{
    if (free_timers) {
	Timer_Entry *self = free_timers;

	free_timers = self->next;
	return self;
    } else
	return (Timer_Entry *) malloc(sizeof(Timer_Entry));
}

static void
free_timer(Timer_Entry * self)
{
    self->next = free_timers;
    free_timers = self;
}

static void restart_timers(void);

static void
wakeup_call(int signo)
{
    Timer_Entry *self = active_timers;
    Timer_Proc proc = self->proc;
    Timer_ID id = self->id;
    Timer_Data data = self->data;

    active_timers = active_timers->next;
    free_timer(self);
    restart_timers();
    if (proc)
	(*proc) (id, data);
}


static void
virtual_wakeup_call(int signo)
{
    Timer_Entry *self = virtual_timer;
    Timer_Proc proc = self->proc;
    Timer_ID id = self->id;
    Timer_Data data = self->data;

    virtual_timer = 0;
    free_timer(self);
    if (proc)
	(*proc) (id, data);
}

static void
stop_timers()
{
    alarm(0);
    signal(SIGALRM, SIG_IGN);
    signal(SIGALRM, wakeup_call);

    {
	struct itimerval itimer, oitimer;

	itimer.it_value.tv_sec = 0;
	itimer.it_value.tv_usec = 0;
	itimer.it_interval.tv_sec = 0;
	itimer.it_interval.tv_usec = 0;

	setitimer(ITIMER_VIRTUAL, &itimer, &oitimer);
	signal(SIGVTALRM, SIG_IGN);
	signal(SIGVTALRM, virtual_wakeup_call);
	if (virtual_timer)
	    virtual_timer->when = oitimer.it_value.tv_sec;
    }
}

static void
restart_timers()
{
    if (active_timers) {
	time_t now = time(0);

	signal(SIGALRM, wakeup_call);

	if (now < active_timers->when)	/* first timer is in the future */
	    alarm(active_timers->when - now);
	else
	    kill(getpid(), SIGALRM);	/* we're already late... */
    }

    if (virtual_timer) {
	signal(SIGVTALRM, virtual_wakeup_call);

	if (virtual_timer->when > 0) {
	    struct itimerval itimer;

	    itimer.it_value.tv_sec = virtual_timer->when;
	    itimer.it_value.tv_usec = 0;
	    itimer.it_interval.tv_sec = 0;
	    itimer.it_interval.tv_usec = 0;

	    setitimer(ITIMER_VIRTUAL, &itimer, 0);
	} else
	    kill(getpid(), SIGVTALRM);
    }
}

Timer_ID
set_timer(unsigned seconds, Timer_Proc proc, Timer_Data data)
{
    Timer_Entry *self = allocate_timer();
    Timer_Entry **t;

    self->id = next_id++;
    self->when = time(0) + seconds;
    self->proc = proc;
    self->data = data;

    stop_timers();

    t = &active_timers;
    while (*t && self->when >= (*t)->when)
	t = &((*t)->next);
    self->next = *t;
    *t = self;

    restart_timers();

    return self->id;
}

Timer_ID
set_virtual_timer(unsigned seconds, Timer_Proc proc, Timer_Data data)
{

    if (virtual_timer)
	return -1;

    stop_timers();

    virtual_timer = allocate_timer();
    virtual_timer->id = next_id++;
    virtual_timer->when = seconds;
    virtual_timer->proc = proc;
    virtual_timer->data = data;

    restart_timers();

    return virtual_timer->id;
}

unsigned
timer_wakeup_interval(Timer_ID id)
{
    Timer_Entry *t;


    if (virtual_timer && virtual_timer->id == id) {
	struct itimerval itimer;

	getitimer(ITIMER_VIRTUAL, &itimer);
	return itimer.it_value.tv_sec;
    }

    for (t = active_timers; t; t = t->next)
	if (t->id == id)
	    return t->when - time(0);;

    return 0;
}

void
timer_sleep(unsigned seconds)
{
    set_timer(seconds, 0, 0);
    pause();
}

int
cancel_timer(Timer_ID id)
{
    Timer_Entry **t = &active_timers;
    int found = 0;

    stop_timers();

    if (virtual_timer && virtual_timer->id == id) {
	free_timer(virtual_timer);
	virtual_timer = 0;
	found = 1;
    } else {
	while (*t) {
	    if ((*t)->id == id) {
		Timer_Entry *tt = *t;

		*t = tt->next;
		found = 1;
		free_timer(tt);
		break;
	    }
	    t = &((*t)->next);
	}
    }

    restart_timers();

    return found;
}

void
reenable_timers(void)
{
    sigset_t sigs;
    
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGALRM);
    sigprocmask(SIG_UNBLOCK, &sigs, 0);
}
