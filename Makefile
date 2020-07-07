CC=gcc
C_FLAGS=--std=gnu11 -Wall -pedantic -g

all: http.o http-webhook.o http-webhook 

http-webhook: http-webhook.o http.o
	@$(CC) $(C_FLAGS) http-webhook.o http.o -g -o http-webhook

http-webhook.o: main.c
	@$(CC) $(C_FLAGS) -c main.c -o http-webhook.o

http.o: http.h http.c
	@$(CC) $(C_FLAGS) -c http.c -o http.o

.PHONY: clean debug

debug: all

clean:
	rm -f http.o http-webhook.o http-webhook vgcore*
