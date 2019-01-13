CFILES = $(wildcard *.c)
OBJS = $(CFILES:.c=.o)
OUT = $(CFILES:.c=)
CC ?= gcc
CFLAGS = -Wall -DDEBUG -g3

all: $(OUT)

$(OUT): $(CFILES)
	$(CC) $(CFLAGS) main.c -lreadline -o main

clean:
	rm -f $(OBJS) $(OUT)