CC=gcc
DEBUG = 
CFLAGS= -g -DDEBUG
LDFLAGS=-lpthread -lm -lz

SRC = zmalloc.c test.c 
TARGET = test
LIB = libmemory.a

OBJ := $(SRC:.c=.o)

all:$(TARGET) lib

$(TARGET):$(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^	
%.o:%.c
	$(CC) $(CFLAGS) $(DEBUG) -c  $<

lib:
	ar cr $(LIB) zmalloc.o
	mv $(LIB)  ../lib/

.PHONY:clean

clean:
	rm -f $(OBJ) $(TARGET) $(OBJE)
	rm -rf ../lib/$(LIB)
