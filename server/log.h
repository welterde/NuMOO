#include <stdio.h>

#include "config.h"
#include "structures.h"

extern void set_log_file(FILE *);

extern void oklog(const char *,...) FORMAT(printf,1,2);
extern void errlog(const char *,...) FORMAT(printf,1,2);
extern void log_perror(const char *);

extern void reset_command_history(void);
extern void log_command_history(void);
extern void add_command_to_history(Objid player, const char *command);


#define log_report_progress()  ((--log_pcount <= 0) && log_report_progress_cktime())

extern int log_pcount;
extern int log_report_progress_cktime(void);
