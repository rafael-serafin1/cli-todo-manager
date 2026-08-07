FILES = main.c files/configure.c utils/utils.c main/parse.c index.c files/todofile.c files/count.c files/config.c
FILE = main.c
TODO = ./bin/todo

comp: 
	g++ $(FILES) -o bin/todo

expd:
	g++ -E $(FILE) -o bin/expanded.i

sep:
	g++ -c main.c
	g++ -c files/configure.c
	g++ -c utils/utils.c
	g++ -c main/parse.c 
	g++ -c index.c

all: comp
	$(TODO) help

debug:
	todo add "Bake a cake"
	todo add "Make another mistake"
	todo add "Go in a vacation"
	todo check "1"
	todo ongoing "2"