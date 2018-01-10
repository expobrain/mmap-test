CC      = gcc
LD      = gcc
CFLAGS  = -ggdb3 -Wall -Wextra -Werror -std=gnu11 -O3
LDFLAGS = -Wl,--no-undefined

OBJ = mmap-test.o multi-mmap-test.o
TESTS = mmap-test multi-mmap-test

all: $(TESTS)

mmap-test: mmap-test.o

multi-mmap-test: multi-mmap-test.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ)
	rm -f $(TESTS)

.PHONY: clean
