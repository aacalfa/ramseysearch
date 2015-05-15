CC = gcc
TOP = .
INCLUDES = -I$(TOP) -I../utils/ -I../socket/client
CFLAGS = -g
COMPILE = $(CC) $(INCLUDES) $(CFLAGS)

INCL=$(UTILS)/jrb.h $(UTILS)/jval.h $(UTILS)/dllist.h $(UTILS)/fifo.h $(UTILS)/graph_utils.h $(UTILS)/msg.h $(UTILS)/utils.h ../socket/client/client.h
COMMON=.
PRED=.
UTILS=../utils
CLIENT=../socket/client
BOBJ=$(UTILS)/jval.o $(UTILS)/jrb.o $(UTILS)/dllist.o $(UTILS)/fifo.o $(UTILS)/graph_utils.o ./clique_count.o ./taboo_search_client.o $(UTILS)/utils.o $(CLIENT)/client.o
LIBS= -lm 

PROGRAMS = simple_taboo_search-7 test_clique_count sim_annealing

all: $(PROGRAMS)

simple_taboo_search-7: simple_taboo_search-7.c $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o simple_taboo_search-7 simple_taboo_search-7.c $(BOBJ) $(LIBS)

test_clique_count: test_clique_count.c ../utils/graph_utils.h $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o test_clique_count test_clique_count.c $(BOBJ) $(LIBS)

sim_annealing: sim_annealing.c $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o sim_annealing sim_annealing.c $(BOBJ) $(LIBS)

fifo.o: fifo.c fifo.h jrb.h jval.h dllist.h
	$(COMPILE) ${INCLUDES} -c fifo.c

clique_count.o: clique_count.c clique_count.h
	$(COMPILE) ${INCLUDES} -c clique_count.c

taboo_search_client.o: taboo_search_client.c ../utils/graph_utils.h
	$(COMPILE) ${INCLUDES} -c taboo_search_client.c

clean:
	rm -f *.o core
