CC = gcc
INCLUDES = -I../ -I./codelets -I.
LIBFLAGS = -Lcodelets
LDFLAGS = -lcodelets -lm -lfl
CFLAGS = -std=c99 -g -O2 $(INCLUDES)

.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) -c $<

SRC = small.c utils.c 
OBJ = $(addsuffix .o, $(basename $(SRC)))

.PHONY: all
all: wht

scanner.c: scanner.l
	flex -X --header-file=scanner.h -o scanner.c scanner.l

scanner.o: scanner.c 

wht: wht.c scanner.o $(OBJ)
	$(CC) $(CFLAGS) $(LIBFLAGS) $(OBJ) scanner.o $< -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f *.o scanner.c scanner.h
