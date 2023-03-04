CC=gcc

compile: clean build
all: compile run

clean:
	rm -rf build/

build:
	mkdir build/
	$(CC) -Wall -pedantic -o build/cpomo *.c -I . -lncurses

run:
	./build/cpomo
