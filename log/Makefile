CC=gcc
DEBUG = -g
CFLAGS= -lpthread -lm -lz
LDFLAGS=

SRC = log.c

TARGET = t

OBJ := $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^	
%.o:%.c
	$(CC) -o $@ -c $<

.PHONY:clean

clean:
	rm -f $(OBJ) $(TARGET) $(OBJE)
