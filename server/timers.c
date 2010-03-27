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
	Timer_Entry *this = free_timers;

	free_timers = this->next;
	return this;
    } else
	return (Timer_Entry *) malloc(sizeof(Timer_Entry));
}

static void
free_timer(Timer_Entry * this)
{
    this->next = free_timers;
    free_timers = this;
}

static void restart_timers(void);

static void
wakeup_call(int signo)
{
    Timer_Entry *this = active_timers;
    Timer_Proc proc = this->proc;
    Timer_ID id = this->id;
    Timer_Data data = this->data;

    active_timers = active_timers->next;
    free_timer(this);
    restart_timers();
    if (proc)
	(*proc) (id, data);
}


static void
virtual_wakeup_call(int signo)
{
    Timer_Entry *this = virtual_timer;
    Timer_Proc proc = this->proc;
    Timer_ID id = this->id;
    Timer_Data data = this->data;

    virtual_timer = 0;
    free_timer(this);
    if (proc)
	(*proc) (id, data);
}

static void
stop_timers()
{
    struct itimerval itimer, oitimer;

    alarm(0);                   /* cancel outstanding alarm */
    signal(SIGALRM, SIG_IGN);   /* XXX FIXME use sigaction */
    signal(SIGALRM, wakeup_call); /* XXX FIXME use sigaction */

    itimer.it_value.tv_sec = 0;
    itimer.it_value.tv_usec = 0;
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 0;
    
    setitimer(ITIMER_PROF, &itimer, &oitimer);
    signal(SIGPROF, SIG_IGN); /* XXX FIXME use sigaction */
    signal(SIGPROF, virtual_wakeup_call); /* XXX FIXME use sigaction */
    if (virtual_timer)
        virtual_timer->when = oitimer.it_value.tv_sec;
}

static void
restart_timers()
{
    if (active_timers) {
	time_t now = time(0);

	signal(SIGALRM, wakeup_call); /* XXX FIXME use sigaction */

	if (now < active_timers->when)	/* first timer is in the future */
	    alarm(active_timers->when - now);
	else
	    kill(getpid(), SIGALRM);	/* we're already late... */
    }

    if (virtual_timer) {
	signal(SIGPROF, virtual_wakeup_call); /* XXX FIXME use sigaction */

	if (virtual_timer->when > 0) {
	    struct itimerval itimer;

	    itimer.it_value.tv_sec = virtual_timer->when;
	    itimer.it_value.tv_usec = 0;
	    itimer.it_interval.tv_sec = 0;
	    itimer.it_interval.tv_usec = 0;

	    setitimer(ITIMER_PROF, &itimer, 0);
	} else
	    kill(getpid(), SIGPROF);
    }
}

/* Real-time timer, used by name lookup, checkpoint scheduling, and
   connection timeout */
Timer_ID
set_timer(unsigned seconds, Timer_Proc proc, Timer_Data data)
{
    Timer_Entry *this = allocate_timer();
    Timer_Entry **t;

    this->id = next_id++;
    this->when = time(0) + seconds;
    this->proc = proc;
    this->data = data;

    stop_timers();

    t = &active_timers;
    while (*t && this->when >= (*t)->when)
	t = &((*t)->next);
    this->next = *t;
    *t = this;

    restart_timers();

    return this->id;
}

/* CPU-second timer, used for task timeouts */
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

	getitimer(ITIMER_PROF, &itimer);
	return itimer.it_value.tv_sec;
    }

    for (t = active_timers; t; t = t->next)
	if (t->id == id)
	    return t->when - time(0);;

    return 0;
}

/* timer_sleep is only ever used when retrying dumps.  We'd use
   sleep() but a forked checkpoint could wake it up early */
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
