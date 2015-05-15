SRC_DIRS=\
src/utils \
src/isocheck \
src/simple_taboo_search \
src/socket/client \
src/socket/server

MAKE_DIRS = $(SRC_DIRS)


all: multimake 

multimake:
	@for d in $(MAKE_DIRS);  \
	do          \
		make -C $$d;    \
	done;       \

clean:
	@for d in $(MAKE_DIRS); \
	do          \
		make -C $$d clean;  \
	done;           \

