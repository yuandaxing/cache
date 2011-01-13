CC=gcc
DEBUG = -g
CFLAGS= 
LDFLAGS=-lpthread -lm -lz

SUBDIR=memory log hashtable 
SUBTARGET = subdir
SRC = 

TARGET = cache

OBJ := $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(SUBTARGET)


subdir:
	@for i in $(SUBDIR); do\
		(cd $$i && $(MAKE)) || exit 1; \
	done

%.o:%.c
	$(CC) -o $@ -c $<

.PHONY:clean

clean:
	rm -f $(OBJ) $(TARGET) $(OBJE)
	@for i in $(SUBDIR); do\
		(cd $$i && (make clean)) || exit 1; \
	done
