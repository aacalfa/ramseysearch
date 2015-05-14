CC = gcc
TOP = ../../simple_taboo_search/
INCLUDES = -I$(TOP) -I../../utils -I../../isocheck
CFLAGS = -g
COMPILE = $(CC) $(INCLUDES) $(CFLAGS)

INCL=../../simple_taboo_search/graph_utils.h ../../simple_taboo_search/dllist.h ../../simple_taboo_search/jval.h ../../simple_taboo_search/clique_count.h ../../utils/msg.h ../../isocheck/isomorph.h
COMMON=../../simple_taboo_search
PRED=.
BOBJ=$(COMMON)/graph_utils.o $(COMMON)/dllist.o $(COMMON)/jval.o $(COMMON)/jrb.o $(COMMON)/clique_count.o ../../isocheck/isomorph.o
LIBS= -lm 

PROGRAMS = server

all: $(PROGRAMS)

server: server.c $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o server server.c servertest.c $(BOBJ) $(LIBS)


clean:
	rm -f *.o core server
