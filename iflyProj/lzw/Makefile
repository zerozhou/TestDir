############################################################################
# Makefile for lzw encode/decode library and sample program
############################################################################
CC = gcc
LD = gcc
CFLAGS = -O3 -Wall -pedantic -ansi -c
#CFLAGS = -Wall -pedantic -ansi -c
LDFLAGS = -O3 -o


# libraries
LIBS = -L. -llzw

#Delete 
DEL = rm

all:	 retrieve	 liblzw.a

retrieve: retrieveMain.o iFlySelect.o liblzw.a 
		$(LD) $^ $(LIBS) $(LDFLAGS) $@

retrieveMain.o:	retrieveMain.c  iFlySelect.h 
		$(CC) $(CFLAGS) $<

iFlySelect.o: iFlySelect.c lzw.h
		$(CC) $(CFLAGS) $<

liblzw.a:	lzwencode.o lzwdecode.o bitfile.o
		ar crv liblzw.a lzwencode.o lzwdecode.o bitfile.o
		ranlib liblzw.a

lzwencode.o:	lzwencode.c lzw.h bitfile.h
		$(CC) $(CFLAGS) $<

lzwdecode.o:	lzwdecode.c lzw.h bitfile.h
		$(CC) $(CFLAGS) $<

bitfile.o:	bitfile.c bitfile.h
		$(CC) $(CFLAGS) $<

clean:
		$(DEL) *.o
		$(DEL) *.a
	#	$(DEL) compress
