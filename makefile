CC=gcc
CFLAGS=-I.

myshell.o: myshell.c
	$(CC) -c -o $@ $< $(CFLAGS)

mysh: myshell.o
	gcc -o $@ $^ $(CFLAGS)
	rm -f myshell.o

.PHONY: clean

clean:
	rm -f mysh
