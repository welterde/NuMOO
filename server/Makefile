
CC = gcc -g 
LIBS = ucd/libucd.a -lpcre  -lm -lcrypt 
YACC = bison -y

# These are pretty insane.
# CFLAGS = -g -O3 -Wall -W -Wwrite-strings -Wpointer-arith -Wbad-function-cast -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Winline -Wwrite-strings

# No optimization while we're still in development
CFLAGS = -g -Wall -W -Wwrite-strings -Wpointer-arith -Wbad-function-cast -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs

CPPFLAGS = -I. -Iucd 
LDFLAGS  = 

YFLAGS = -d
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) -c

CSRCS = ast.c code_gen.c db_file.c db_io.c db_objects.c			\
	db_properties.c db_verbs.c decompile.c disassemble.c		\
	eval_env.c eval_vm.c exceptions.c execute.c extensions.c	\
	functions.c keywords.c list.c log.c match.c md5.c		\
	name_lookup.c network.c net_mplex.c net_proto.c numbers.c	\
	objects.c parse_cmd.c pattern.c program.c property.c		\
	ref_count.c server.c storage.c streams.c str_intern.c		\
	sym_table.c tasks.c timers.c unparse.c utf.c utf-ctype.c	\
	utils.c verbs.c version.c waif.c

OPT_NET_SRCS = net_single.c net_multi.c net_mp_selct.c \
	net_tcp.c net_bsd_tcp.c net_bsd_lcl.c

OPT_NET_OBJS = $(OPT_NET_SRCS:.c=.o)

OPT_CSRCS = $(OPT_NET_SRCS)

YSRCS = parser.y

HDRS = ast.h bf_register.h code_gen.h db.h db_io.h db_private.h		\
	decompile.h db_tune.h disassemble.h eval_env.h eval_vm.h	\
	exceptions.h execute.h functions.h keywords.h list.h log.h	\
	match.h md5.h name_lookup.h network.h net_mplex.h net_multi.h	\
	net_proto.h numbers.h opcode.h options.h parse_cmd.h parser.h	\
	pattern.h program.h random.h ref_count.h server.h storage.h	\
	streams.h structures.h str_intern.h sym_table.h tasks.h		\
	timers.h tokens.h unparse.h utf-ctype.h utils.h verbs.h		\
	version.h


CLIENT_SRCS = client_bsd.c

ALL_CSRCS = $(CSRCS) $(OPT_CSRCS) $(CLIENT_SRCS)

SRCS = $(ALL_CSRCS) keywords.gperf $(YSRCS) $(HDRS) $(SYSHDRS)

DISTFILES = $(SRCS) restart \
	Minimal.db README.Minimal \
	ChangeLog.txt README \
	MOOCodeSequences.txt AddingNewMOOTypes.txt 

COBJS = $(CSRCS:.c=.o)

YOBJS = $(YSRCS:.y=.o)

OBJS = $(COBJS) $(YOBJS) 

all:
	cd ucd && $(MAKE) all
	$(MAKE) moo

moo:	$(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

client_bsd: client_bsd.o
	$(CC) $(CFLAGS) client_bsd.o $(LIBRARIES) -o $@

# This rule gets around some "make"s' desire to `derive' it from `restart.sh'.
restart:
	touch restart

y.tab.h: parser.o
	touch y.tab.h

keywords.c: keywords.gperf
	gperf -CtT --ignore-case keywords.gperf \
		| sed -e 's/#include <ctype.h>/#include "utf-ctype.h"/' \
		> keywords.c

clean:
	rm -f $(OBJS) $(OPT_NET_OBJS) core parser.c y.tab.c y.tab.h y.output makedep eddep

distclean:	clean
	cd ucd && $(MAKE) clean

depend: ${ALL_CSRCS}
	rm -f eddep makedep
	gcc -MM ${CPPFLAGS} ${ALL_CSRCS} | sed -e '/:$$/d' > makedep
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	rm -f eddep makedep

# for flymake's benefit.  only works with gcc.
check-syntax:
	gcc -Wall -fsyntax-only $(CHK_SOURCES)	

.SUFFIXES: .c .o .i .s
.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} -c -o $@ $<
.c.i:
	${CC} ${CFLAGS} ${CPPFLAGS} -E -o $@ $<
.c.s:
	${CC} ${CFLAGS} ${CPPFLAGS} -S -o $@ $<

# Have to do this one manually, since make depend cannot hack yacc files.
parser.o:	utf-ctype.h    \
		ast.h code_gen.h config.h functions.h \
		keywords.h list.h log.h numbers.h opcode.h parser.h program.h \
		storage.h streams.h structures.h sym_table.h utils.h version.h

# Must do these specially, since they depend upon C preprocessor options.
network.o: 	net_single.o net_multi.o
net_proto.o:	net_bsd_tcp.o net_bsd_lcl.o 
net_mplex.o:	net_mp_selct.o 

$(OPT_NET_OBJS):
	touch $@

# DO NOT DELETE THIS LINE -- 'make depend' replaces everything below it.
ast.o: ast.c  config.h ast.h parser.h program.h structures.h \
   version.h sym_table.h list.h log.h storage.h ref_count.h \
  utils.h execute.h db.h opcode.h options.h parse_cmd.h
code_gen.o: code_gen.c ast.h config.h parser.h program.h structures.h \
   version.h sym_table.h exceptions.h opcode.h options.h \
  storage.h  ref_count.h str_intern.h utils.h execute.h db.h \
  parse_cmd.h 
db_file.o: db_file.c  config.h   \
    db.h program.h structures.h version.h db_io.h \
  db_private.h exceptions.h list.h log.h options.h server.h network.h \
  storage.h ref_count.h streams.h str_intern.h tasks.h execute.h opcode.h \
  parse_cmd.h timers.h 
db_io.o: db_io.c config.h     \
  db_io.h program.h structures.h version.h db_private.h exceptions.h \
  list.h log.h numbers.h parser.h storage.h ref_count.h streams.h \
  str_intern.h unparse.h
db_objects.o: db_objects.c config.h db.h program.h structures.h \
   version.h db_private.h exceptions.h list.h storage.h \
   ref_count.h utils.h execute.h opcode.h options.h \
  parse_cmd.h
db_properties.o: db_properties.c config.h db.h program.h structures.h \
   version.h db_private.h exceptions.h list.h storage.h \
   ref_count.h utils.h execute.h opcode.h options.h \
  parse_cmd.h
db_verbs.o: db_verbs.c  config.h  db.h program.h \
  structures.h  version.h db_private.h exceptions.h db_tune.h \
  list.h log.h parse_cmd.h storage.h ref_count.h utils.h execute.h \
  opcode.h options.h
decompile.o: decompile.c ast.h config.h parser.h program.h structures.h \
   version.h sym_table.h decompile.h exceptions.h opcode.h \
  options.h storage.h  ref_count.h utils.h execute.h db.h \
  parse_cmd.h
disassemble.o: disassemble.c  config.h bf_register.h db.h \
  program.h structures.h version.h functions.h execute.h opcode.h \
  options.h parse_cmd.h list.h storage.h  ref_count.h \
  streams.h unparse.h utils.h verbs.h
eval_env.o: eval_env.c config.h eval_env.h structures.h  \
  version.h storage.h  ref_count.h sym_table.h utils.h \
  execute.h db.h program.h opcode.h options.h parse_cmd.h
eval_vm.o: eval_vm.c config.h db_io.h program.h structures.h  \
  version.h decompile.h ast.h parser.h sym_table.h eval_vm.h execute.h \
  db.h opcode.h options.h parse_cmd.h log.h storage.h  \
  ref_count.h tasks.h
exceptions.o: exceptions.c exceptions.h config.h
execute.o: execute.c  config.h db.h program.h structures.h \
   version.h db_io.h decompile.h ast.h parser.h sym_table.h \
  eval_env.h eval_vm.h execute.h opcode.h options.h parse_cmd.h \
  exceptions.h functions.h list.h log.h numbers.h server.h network.h \
  storage.h ref_count.h streams.h tasks.h timers.h  utf.h \
  utils.h
extensions.o: extensions.c bf_register.h functions.h  config.h \
  execute.h db.h program.h structures.h version.h opcode.h options.h \
  parse_cmd.h db_tune.h utils.h
functions.o: functions.c  config.h bf_register.h db_io.h \
  program.h structures.h  version.h functions.h execute.h db.h \
  opcode.h options.h parse_cmd.h list.h log.h server.h network.h \
  storage.h  ref_count.h streams.h unparse.h utils.h
keywords.o: keywords.c utf-ctype.h config.h  keywords.h \
  structures.h  version.h tokens.h ast.h parser.h program.h \
  sym_table.h y.tab.h utils.h execute.h db.h opcode.h options.h \
  parse_cmd.h
list.o: list.c utf-ctype.h config.h   bf_register.h \
  exceptions.h functions.h  execute.h db.h program.h \
  structures.h version.h opcode.h options.h parse_cmd.h list.h log.h \
  md5.h pattern.h random.h ref_count.h streams.h storage.h unparse.h \
  ucd/ucd.h utf.h utils.h
log.o: log.c  config.h    \
  bf_register.h functions.h execute.h db.h program.h structures.h \
  version.h opcode.h options.h parse_cmd.h log.h storage.h ref_count.h \
  streams.h utils.h
match.o: match.c  config.h  db.h program.h \
  structures.h  version.h exceptions.h match.h parse_cmd.h \
  storage.h ref_count.h unparse.h utils.h execute.h opcode.h options.h
md5.o: md5.c  config.h md5.h
name_lookup.o: name_lookup.c options.h config.h   \
     \
   log.h  structures.h server.h network.h storage.h \
  ref_count.h timers.h 
network.o: network.c options.h config.h net_multi.c utf-ctype.h \
       \
  exceptions.h list.h structures.h log.h net_mplex.h net_multi.h \
  net_proto.h network.h server.h streams.h storage.h ref_count.h timers.h \
   utf.h utils.h execute.h db.h program.h version.h opcode.h \
  parse_cmd.h
net_mplex.o: net_mplex.c options.h config.h net_mp_selct.c  \
   log.h  structures.h net_mplex.h
net_proto.o: net_proto.c options.h config.h net_bsd_tcp.c \
       \
  list.h structures.h  log.h name_lookup.h net_proto.h server.h \
  network.h streams.h timers.h  utils.h execute.h db.h program.h \
  version.h opcode.h parse_cmd.h net_tcp.c
numbers.o: numbers.c   config.h   \
  functions.h  execute.h db.h program.h structures.h version.h \
  opcode.h options.h parse_cmd.h log.h random.h storage.h ref_count.h \
  utils.h bf_register.h
objects.o: objects.c db.h config.h program.h structures.h  \
  version.h db_io.h exceptions.h execute.h opcode.h options.h parse_cmd.h \
  functions.h list.h numbers.h server.h network.h storage.h \
   ref_count.h utils.h
parse_cmd.o: parse_cmd.c utf-ctype.h config.h   \
    db.h program.h structures.h version.h list.h \
  match.h parse_cmd.h storage.h ref_count.h utils.h execute.h opcode.h \
  options.h
pattern.o: pattern.c  config.h  pattern.h streams.h \
  utf.h storage.h structures.h ref_count.h exceptions.h
program.o: program.c ast.h config.h parser.h program.h structures.h \
   version.h sym_table.h exceptions.h list.h storage.h \
   ref_count.h utils.h execute.h db.h opcode.h options.h \
  parse_cmd.h
property.o: property.c db.h config.h program.h structures.h  \
  version.h functions.h execute.h opcode.h options.h parse_cmd.h list.h \
  storage.h  ref_count.h utils.h
ref_count.o: ref_count.c config.h exceptions.h ref_count.h storage.h \
   structures.h 
server.o: server.c  config.h    \
    db.h program.h \
  structures.h version.h db_io.h disassemble.h execute.h opcode.h \
  options.h parse_cmd.h functions.h list.h log.h network.h server.h \
  parser.h random.h storage.h ref_count.h streams.h tasks.h timers.h \
   unparse.h utils.h
storage.o: storage.c  config.h  exceptions.h list.h \
  structures.h  options.h ref_count.h storage.h utils.h \
  execute.h db.h program.h version.h opcode.h parse_cmd.h
streams.o: streams.c  config.h   log.h \
  structures.h storage.h ref_count.h streams.h utf.h
str_intern.o: str_intern.c  config.h  log.h \
   structures.h storage.h ref_count.h str_intern.h utils.h \
  execute.h db.h program.h version.h opcode.h options.h parse_cmd.h
sym_table.o: sym_table.c  config.h ast.h parser.h program.h \
  structures.h version.h sym_table.h exceptions.h log.h storage.h \
   ref_count.h utils.h execute.h db.h opcode.h options.h \
  parse_cmd.h
tasks.o: tasks.c  config.h  db.h program.h \
  structures.h  version.h db_io.h decompile.h ast.h parser.h \
  sym_table.h eval_env.h eval_vm.h execute.h opcode.h options.h \
  parse_cmd.h exceptions.h functions.h list.h log.h match.h random.h \
  server.h network.h storage.h ref_count.h streams.h tasks.h utils.h \
  verbs.h
timers.o: timers.c  config.h   \
  options.h    timers.h
unparse.o: unparse.c utf-ctype.h config.h  ast.h parser.h \
  program.h structures.h version.h sym_table.h decompile.h exceptions.h \
  functions.h execute.h db.h opcode.h options.h parse_cmd.h keywords.h \
  list.h log.h unparse.h storage.h  ref_count.h streams.h \
  utils.h
utf.o: utf.c utf.h streams.h config.h
utf-ctype.o: utf-ctype.c config.h ucd/ucd.h utf-ctype.h
utils.o: utils.c utf-ctype.h config.h   db.h \
  program.h structures.h version.h db_io.h exceptions.h list.h log.h \
  match.h numbers.h ref_count.h server.h network.h options.h storage.h \
  streams.h utf.h utils.h execute.h opcode.h parse_cmd.h
verbs.o: verbs.c  config.h db.h program.h structures.h \
   version.h exceptions.h execute.h opcode.h options.h \
  parse_cmd.h functions.h list.h log.h match.h parser.h server.h \
  network.h storage.h ref_count.h unparse.h utils.h verbs.h
version.o: version.c config.h version.h
net_single.o: net_single.c utf-ctype.h config.h  \
   log.h structures.h network.h options.h server.h streams.h \
  utils.h execute.h db.h program.h version.h opcode.h parse_cmd.h
net_multi.o: net_multi.c utf-ctype.h config.h \
       exceptions.h \
  list.h structures.h log.h net_mplex.h net_multi.h net_proto.h options.h \
  network.h server.h streams.h storage.h ref_count.h timers.h  \
  utf.h utils.h execute.h db.h program.h version.h opcode.h parse_cmd.h
net_mp_selct.o: net_mp_selct.c  config.h \
  options.h  log.h  structures.h net_mplex.h
net_tcp.o: net_tcp.c
net_bsd_tcp.o: net_bsd_tcp.c config.h  \
      list.h structures.h \
   log.h name_lookup.h net_proto.h options.h server.h network.h \
  streams.h timers.h  utils.h execute.h db.h program.h version.h \
  opcode.h parse_cmd.h net_tcp.c
net_bsd_lcl.o: net_bsd_lcl.c  config.h   \
   log.h structures.h net_proto.h options.h storage.h \
  ref_count.h utils.h execute.h db.h program.h version.h opcode.h \
  parse_cmd.h
client_bsd.o: client_bsd.c  config.h   \
   options.h  
