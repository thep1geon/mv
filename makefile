CC = gcc
FLAGS = -Wall -Wextra

main: main.c
	$(CC) main.c $(FLAGS) -o mv
