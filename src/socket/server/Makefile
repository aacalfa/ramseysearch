CC = gcc
TOP = ../../simple_taboo_search/
INCLUDES = -I$(TOP) -I../../utils -I../../isocheck
CFLAGS = -g
COMPILE = $(CC) $(INCLUDES) $(CFLAGS)

UTILS=../../utils
INCL=$(UTILS)/graph_utils.h $(UTILS)/dllist.h $(UTILS)/jval.h $(TOP)/clique_count.h $(UTILS)/msg.h ../../isocheck/isomorph.h
COMMON=../../simple_taboo_search
PRED=.
BOBJ=$(UTILS)/graph_utils.o $(UTILS)/dllist.o $(UTILS)/jval.o $(UTILS)/jrb.o $(COMMON)/clique_count.o ../../isocheck/isomorph.o
LIBS= -lm 

PROGRAMS = server

all: $(PROGRAMS)

server: server.c $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o server server.c servertest.c $(BOBJ) $(LIBS)


clean:
	rm -f *.o core server
