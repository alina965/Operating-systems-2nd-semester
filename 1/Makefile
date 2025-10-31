CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBRARY = libmythread.a
SOURCES = my-thread.c
OBJECTS = $(SOURCES:.c=.o)
EXAMPLE = example

all: $(LIBRARY) $(EXAMPLE)

$(LIBRARY): $(OBJECTS)
	ar rcs $@ $^

$(EXAMPLE): $(EXAMPLE).o $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $< -L. -lmythread

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(LIBRARY) $(EXAMPLE) *.o

.PHONY: all clean