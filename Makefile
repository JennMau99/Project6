CC = gcc
CFLAGS = -g -Wall -pedantic
MAIN = parseline
OBJS = parseline.o execute.o
all : $(MAIN)



$(MAIN) : $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

parseline.o : parseline.c header.h
	$(CC) $(CFLAGS) -c parseline.c

execute.o : execute.c header.h
	$(CC) $(CFLAGS) -c execute.c 

clean :
	rm *.o $(MAIN) core

