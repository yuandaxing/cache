AS	=as
LD	=ld
CC	=g++
AR	=ar


MYSQL_INCLUDE_PATH = 
MYSQL_LIB_PATH = 
MYSQL_LIB = 

CFLAGS = -g  -Wall
CFLAGS += -I$(MYSQL_INCLUDE_PATH) -L$(MYSQL_LIB_PATH) $(MYSQL_LIB) 

SRCS = test.c detach.c
OBJS = $(SRCS:.c=.o)

EXECUT_TAGS := demo
LIB_TAGS := libDatabase.a

all:$(EXECUT_TAGS)
${EXECUT_TAGS}:${OBJS}
	$(AR) -rc ${LIB_TAGS} $^
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGS) -o $@ $<

.SUFFIXES:
.SUFFIXES: .c .o

.PHONY:clean
clean: 
	$(RM) *.o *.a
	$(RM) ${EXECUT_TAGS}



