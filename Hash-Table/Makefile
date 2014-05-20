CC = gcc
CFLAGS = -Wall -c
LDFLAGS = -Wall -DDEBUG -o
RM = rm -f
all: hash

hash: main.o hashtable.o 
	$(CC) $(LDFLAGS) $@ $^

main.o: main.c hashtable.h
	$(CC) $(CFLAGS) $<

hashtable.o: hashtable.c debug.h
	$(CC) $(CFLAGS) $<
clean:
	$(RM) *.o  hash

