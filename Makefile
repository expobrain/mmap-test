CC      = gcc
LD      = gcc
CFLAGS  = -ggdb3 -Wall -Wextra -Werror -std=gnu11
LDFLAGS = -Wl,--no-undefined

OBJ = mmap-test.o

all: mmap-test

mmap-test: $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $<

clean:
	rm $(OBJ)
	rm mmap-test

.PHONY: clean
