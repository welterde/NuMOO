#ifndef Timers_H
#define Timers_H 1

#include <time.h>

typedef int Timer_ID;
typedef void *Timer_Data;
typedef void (*Timer_Proc) (Timer_ID, Timer_Data);

extern Timer_ID set_timer(unsigned, Timer_Proc, Timer_Data);
extern Timer_ID set_virtual_timer(unsigned, Timer_Proc, Timer_Data);
extern int cancel_timer(Timer_ID);
extern void reenable_timers(void);
extern unsigned timer_wakeup_interval(Timer_ID);
extern void timer_sleep(unsigned seconds);
extern int virtual_timer_available(void);

#endif				/* !Timers_H */
