CFILES = $(wildcard *.c)
OBJS = $(CFILES:.c=.o)
OUT = $(CFILES:.c=)
CC ?= gcc
CFLAGS = -Wall -DDEBUG -g3

all: $(OUT)

%.o: %c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(OUT)