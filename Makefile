FILES = main.c files/configure.c utils/utils.c main/parse.c index.c files/todofile.c files/count.c files/config.c
FILE = main.c

comp: 
	g++ $(FILES) -o todo

expd:
	g++ -E $(FILE) -o expanded.i

sep:
	g++ -c main.c
	g++ -c files/configure.c
	g++ -c utils/utils.c
	g++ -c main/parse.c 
	g++ -c index.c

all:
	g++ $(FILES) -o todo
	./todo help