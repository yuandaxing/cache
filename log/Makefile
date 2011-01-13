CC=gcc
DEBUG = -g
CFLAGS=-DLOG_MUTEX
LDFLAGS=-lpthread -lm -lz

SRC = log.c test.c
LIB = liblog.a

TARGET = test

OBJ := $(SRC:.c=.o)

all:$(TARGET) $(LIB)

$(TARGET):$(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^	
%.o:%.c
	$(CC) $(CFLAGS) $(DEBUG)  -c  $<
$(LIB):
	ar crv $(LIB) log.o
	mv $(LIB) ../lib/

.PHONY:clean

clean:
	rm -f $(OBJ) $(TARGET) $(OBJE)
