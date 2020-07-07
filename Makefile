CC=gcc
C_FLAGS=--std=gnu11 -Wall -pedantic -g

all: actions.o http.o filter.o http-webhook.o http-webhook 

http-webhook: http-webhook.o http.o
	@$(CC) $(C_FLAGS) http-webhook.o actions.o http.o filter.o -g -o http-webhook

http-webhook.o: main.c
	@$(CC) $(C_FLAGS) -c main.c -o http-webhook.o

filter.o: filter.h filter.c
	@$(CC) $(C_FLAGS) -c filter.c -o filter.o

http.o: http.h http.c
	@$(CC) $(C_FLAGS) -c http.c -o http.o

actions.o: actions.h actions.c
	@$(CC) $(C_FLAGS) -c actions.c -o actions.o

.PHONY: clean debug

debug: all

clean:
	rm -f *.o http-webhook vgcore*
