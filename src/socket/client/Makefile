CC = gcc
TOP = ../../simple_taboo_search/
INCLUDES = -I$(TOP) -I../../utils
CFLAGS = -g
COMPILE = $(CC) $(INCLUDES) $(CFLAGS)

INCL=../../simple_taboo_search/graph_utils.h ../../utils/utils.h ../../utils/msg.h
COMMON=../../simple_taboo_search
PRED=.
BOBJ=$(COMMON)/graph_utils.o ../../utils/utils.o
LIBS= -lm 

PROGRAMS = client

all: $(PROGRAMS)

client: client.c $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o client client.c clienttest.c $(BOBJ) $(LIBS)


clean:
	rm -f *.o core client
