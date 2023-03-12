CC=gcc

compile: clean build
all: compile install run

clean:
	rm -rf build/

build:
	mkdir build/
	$(CC) -Wall -pedantic -o build/cpomo *.c -I . -lncurses

run:
	./build/cpomo

install:
	cp ./build/cpomo ~/.local/bin/ || cp ./build/cpomo /usr/local/bin
