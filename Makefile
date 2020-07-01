CC=gcc
C_FLAGS=--std=gnu11

all: http-webhook.o http-webhook 

http-webhook: http-webhook.o
	@$(CC) http-webhook.o -o http-webhook

http-webhook.o: main.c
	@$(CC) -c main.c -o http-webhook.o

clean:
	rm -f http-webhook.o http-webhook
