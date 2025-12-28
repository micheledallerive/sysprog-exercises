CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
TARGET = test

SRCS = lib.c test.c custom_tests.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

test: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

check: mdr.o test.o custom_tests.o
	$(CC) $(CFLAGS) -o check mdr.o test.o custom_tests.o
	./check

%.o: %.c lib.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) mdr.o test check

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
