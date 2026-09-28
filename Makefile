CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200112L -g

lookup: lookup.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f lookup
