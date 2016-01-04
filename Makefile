CFLAGS	= -Wall -Werror -Wpedantic -O3 -g
LDADD	= -ldynamic

clo: clo.o
	$(CC) $(CFLAGS) -o $@ $< $(LDADD)
