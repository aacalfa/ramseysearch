CC = gcc
TOP = .
INCLUDES = -I$(TOP) -I../simple_taboo_search
CFLAGS = -g 
COMPILE = $(CC) $(INCLUDES) $(CFLAGS)

INCL=../simple_taboo_search/jrb.h ../simple_taboo_search/jval.h
COMMON=../simple_taboo_search
PRED=.
BOBJ=$(COMMON)/jval.o $(COMMON)/jrb.o
LIBS= -lm 

PROGRAMS = iso_check

all: $(PROGRAMS)


iso_check: iso_check.c isomorph.o $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o iso_check iso_check.c isomorph.o $(BOBJ) $(LIBS)

isomorph.o: isomorph.c ${INCL} 
	$(COMPILE) ${INCLUDES} -c isomorph.c

jrb.o: jrb.c jrb.h jval.h
	$(COMPILE) ${INCLUDES} -c jrb.c

jval.o: jval.c jval.h
	$(COMPILE) ${INCLUDES} -c jval.c

clean:
	rm -f *.o core
