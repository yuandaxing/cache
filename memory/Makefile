CC=gcc
DEBUG = -g -DDEBUG
CFLAGS= -lpthread -lm -lz
LDFLAGS=

SRC = test.c zmalloc.c 
TARGET = test

OBJ := $(SRC:.c=.o)

test:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^	
%.o:%.c
	$(CC) -o $@ -c $(DEBUG) $<

.PHONY:clean

clean:
	rm -f $(OBJ) $(TARGET) $(OBJE)
