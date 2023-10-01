CC = gcc
CFLAGS = -w

all: csh date dir

csh: csh.c word.c
	$(CC) $(CFLAGS) -o csh csh.c word.c

date: date.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/date date.c

dir: dir.c
	$(CC) $(CFLAGS) -o bin/dir dir.c

clean:
	rm -rf csh bin/
