EXEC = ../bin/signal_test

CC = gcc
#
# basic flags setting
#
CFLAGS += -g -I../include
LDFLAGS +=  -lsystemd

%.o:	%.c
	$(CC) $(CFLAGS) -c -o $@ $<

#
# object files
#
OBJS = main.o 

#
# directives
#

all : $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(EXEC)

clean:
	rm -f core *.o $(EXEC)

