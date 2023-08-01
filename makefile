FILES = $(wildcard src/*.c)
FLAGS = -Wall -Wextra -fsanitize=address


main: $(FILES)
	@mkdir -p target
	gcc $(FLAGS) $(FILES) -o target/mv

run: main
	target/mv ./app.mv

clean:
	@rm -rf target
