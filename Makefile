CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread -lrt

TARGET = proj2

SRCS = main.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c main.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(OBJS) $(TARGET)
