FILES = $(wildcard src/*.c)
FLAGS = -Wall -Wextra

main: $(FILES)
	@mkdir -p target
	gcc $(FLAGS) $(FILES) -o target/mv
