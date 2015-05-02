CC = gcc
TOP = .
INCLUDES = -I$(TOP)
CFLAGS = -g
COMPILE = $(CC) $(INCLUDES) $(CFLAGS)

INCL=.
COMMON=.
PRED=.
BOBJ=.

utils.o: utils.c
	$(COMPILE) ${INCLUDES} -c utils.c

clean:
	rm -f *.o core

