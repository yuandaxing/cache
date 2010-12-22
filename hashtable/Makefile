SRCDIR=..
CC=gcc
DEBUG = -g
CFLAGS= -lpthread -lm -lz
LDFLAGS=
INCLUDE=-I $(SRCDIR)/include
LIB = dict.a
SRC = dict.c 
RANLIB=ranlib

TARGET = 



OBJ := $(SRC:.c=.o)

all:$(TARGET) $(LIB)

$(TARGET):$(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^	
%.o:%.c
	$(CC) -c $(INCLUDE) $<
$(LIB):$(OBJ)
	rm -f $@
	ar cr $@ $^
	$(RANLIB) $@

.PHONY:clean

clean:
	rm -f $(OBJ) $(TARGET) $(OBJE) $(LIB)
