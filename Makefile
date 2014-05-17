CC = gcc
CFLAGS = -Wall -std=c99 -o image-evolve -lcairo

all:    
		$(CC) $(CFLAGS) image-evolve.c
