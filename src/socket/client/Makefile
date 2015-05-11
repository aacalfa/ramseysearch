CC = gcc
TOP =.
INCLUDES = -I$(TOP) -I../../utils -I../../simple_taboo_search/
CFLAGS = -g
COMPILE = $(CC) $(INCLUDES) $(CFLAGS)

INCL=../../simple_taboo_search/graph_utils.h ../../utils/utils.h ../../utils/msg.h ./client.h 
COMMON=../../simple_taboo_search
PRED=.
BOBJ=./client.o $(COMMON)/graph_utils.o ../../utils/utils.o $(COMMON)/taboo_search_client.o $(COMMON)/fifo.o $(COMMON)/jrb.o $(COMMON)/jval.o $(COMMON)/dllist.o $(COMMON)/clique_count.o
LIBS= -lm 

PROGRAMS = client

all: $(PROGRAMS)

client: client.c $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o client clienttest.c $(BOBJ) $(LIBS)

client.o: client.c ../../simple_taboo_search/graph_utils.h
	$(COMPILE) ${INCLUDES} -c client.c

clean:
	rm -f *.o core client
