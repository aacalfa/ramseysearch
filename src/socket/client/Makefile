CC = gcc
TOP =.
INCLUDES = -I$(TOP) -I../../utils -I../../simple_taboo_search/
CFLAGS = -g
COMPILE = $(CC) $(INCLUDES) $(CFLAGS)

UTILS=../../utils
INCL=$(UTILS)/graph_utils.h $(UTILS)/utils.h $(UTILS)/msg.h ./client.h 
COMMON=../../simple_taboo_search
PRED=.
BOBJ=$(UTILS)/graph_utils.o $(UTILS)/utils.o $(COMMON)/taboo_search_client.o $(UTILS)/fifo.o $(UTILS)/jrb.o $(UTILS)/jval.o $(UTILS)/dllist.o $(COMMON)/clique_count.o ./client.o
LIBS= -lm 

PROGRAMS = client

all: $(PROGRAMS)

client: client.c $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o client clienttest.c $(BOBJ) $(LIBS)

client.o: client.c ../../utils/graph_utils.h
	$(COMPILE) ${INCLUDES} -c client.c

clean:
	rm -f *.o core client
