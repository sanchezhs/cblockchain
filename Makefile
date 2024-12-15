CC = gcc
CFLAGS = -I./openssl-3.4.0/include -Wall -Wextra
LDFLAGS = -L./openssl-3.4.0/
LDLIBS  = -lcrypto

SOURCES = main.c merkletree.c blockchain.c

main: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $(SOURCES) -o main

