CC = gcc
TOP = .
INCLUDES = -I$(TOP)
CFLAGS = -g -fopenmp
COMPILE = $(CC) $(INCLUDES) $(CFLAGS)

INCL=./jrb.h ./jval.h ./dllist.h ./fifo.h
COMMON=.
PRED=.
BOBJ=$(COMMON)/jval.o $(COMMON)/jrb.o $(COMMON)/dllist.o $(COMMON)/fifo.o $(COMMON)/graph_utils.o $(COMMON)/clique_count.o
LIBS= -lm 

#PROGRAMS = simple_taboo_search-7 test_clique_count
PROGRAMS = simple_taboo_search-7 test_clique_count sim_annealing

all: $(PROGRAMS)

simple_taboo_search-7: simple_taboo_search-7.c $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o simple_taboo_search-7 simple_taboo_search-7.c $(BOBJ) $(LIBS)

test_clique_count: test_clique_count.c graph_utils.h $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o test_clique_count test_clique_count.c $(BOBJ) $(LIBS)

sim_annealing: sim_annealing.c $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o sim_annealing sim_annealing.c $(BOBJ) $(LIBS)

fifo.o: fifo.c fifo.h jrb.h jval.h dllist.h
	$(COMPILE) ${INCLUDES} -c fifo.c

jrb.o: jrb.c jrb.h jval.h
	$(COMPILE) ${INCLUDES} -c jrb.c

jval.o: jval.c jval.h
	$(COMPILE) ${INCLUDES} -c jval.c

dllist.o: dllist.c dllist.h jval.h
	$(COMPILE) ${INCLUDES} -c dllist.c

graph_utils.o: graph_utils.c graph_utils.h
	$(COMPILE) ${INCLUDES} -c graph_utils.c

clique_count.o: clique_count.c clique_count.h
	$(COMPILE) ${INCLUDES} -c clique_count.c

clean:
	rm -f *.o core
