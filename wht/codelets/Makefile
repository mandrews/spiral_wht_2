CC = gcc
CFLAGS = -O2 
INC_FLAGS = -I../ -I../../
SRC = codelets.c s_2.c
OBJ = $(addsuffix .o, $(basename $(SRC)))
LIB = libcodelets.so

.PHONY: all
all: $(LIB)

.c.o:
	$(CC) $(CFLAGS) -fPIC $(INC_FLAGS) -c $<

$(LIB): $(OBJ)
	$(CC) $(CFLAGS) -shared -Wl,-soname,$(LIB) -o $(LIB) $(OBJ) -ldl

.PHONY: clean
clean:
	rm -f *.o $(LIB)

.PHONY: distclean
distclean: clean
	rm -f Makefile 
