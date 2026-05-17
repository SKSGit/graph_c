CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2
LDFLAGS = -lm

main: main.c uthash.h
	$(CC) $(CFLAGS) -o main main.c $(LDFLAGS)

test: main
	bash test_double.sh && bash test_mm.sh && bash test_overflow.sh

clean:
	rm -f main

.PHONY: test clean
