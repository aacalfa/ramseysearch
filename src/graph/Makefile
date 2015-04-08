# define the C compiler to use
CC = gcc

# define global compile-time flags
CFLAGS = -Wall -g -Werror
#CFLAGS = -Wall -g

# define directories containing header files other than /usr/include
INCLUDES = -I../headers

# define library paths in addition to /usr/lib
LFLAGS =

# define any libraries to link into executable:
# Ex: for pthreads: -lpthread
LIBS =

# define Graph sources
SRCS = $(shell ls *.c)

BUILD_DIR = ../build/graph
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
#COMMON_OBJ_DIR = ../../build/common/obj
OBJS = $(SRCS:.c=.o)
#OBJS_BUILD = $(addprefix $(OBJ_DIR)/,$(OBJS)) $(shell ls $(COMMON_OBJ_DIR)/*.o)
OBJS_BUILD = $(addprefix $(OBJ_DIR)/,$(OBJS))
PROJECT = graph

all: dirs $(PROJECT)

dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)
	

$(PROJECT): $(OBJS)
	$(CC) $(OBJS_BUILD) $(INCLUDES) -o $(BIN_DIR)/$(PROJECT) $(CFLAGS) $(LFLAGS) $(LIBS)

%.o : %.c
	$(CC) $< $(CFLAGS) $(INCLUDES) $(LFLAGS) $(LIBS) -c -o $(OBJ_DIR)/$@

clean:
	rm -rf $(BUILD_DIR)
