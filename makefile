CC = gcc
FLAGS = -Wall -Wextra -g

main: main.c
	$(CC) main.c $(FLAGS) -o mv
