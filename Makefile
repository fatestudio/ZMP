CC=gcc

all: main

main: main.o ZEcc.o ZMP.o
	$(CC) -o main main.o ZEcc.o ZMP.o

main.o: main.c
	$(CC) -c main.c

ZEcc.o: ZEcc.c
	$(CC) -c ZEcc.c

ZMP.o: ZMP.c
	$(CC) -c ZMP.c

clean:
	rm -rf *o main

