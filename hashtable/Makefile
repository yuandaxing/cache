SRCDIR=..
CC=gcc
DEBUG = -g
CFLAGS= $(DEBUG) -Wall
LDFLAGS= -L../lib/ -lmemory -llog
INCLUDE=-I $(SRCDIR)/include
LIB = dict.a
SRC = dict.c test.c randgen.c
RANLIB=ranlib

TARGET = test



OBJ := $(SRC:.c=.o)

all:$(OBJ) $(LIB) $(TARGET)

$(TARGET):$(OBJ)
	$(CC) -o $@ $^	$(LDFLAGS) 
%.o:%.c
	$(CC) $(INCLUDE) $(CFLAGS) -c  $<
$(LIB):
	rm -f $@
	ar cr $@ dict.o
	$(RANLIB) $@
	mv $@ ../lib/

.PHONY:clean

clean:
	rm -f $(OBJ) $(TARGET) $(OBJE) $(LIB)
