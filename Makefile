FILES=index.cpp parse/parse.cpp commands/commands.cpp config/config.cpp file/file.cpp

IMPORTANT_FLAGS=-std=c++20 -O2 -finput-charset=UTF-8 -fexec-charset=UTF-8
EXTRA_FLAGS=-Wall

INCLUDE=sqlite3.o

ifeq ($(OS),Windows_NT)
    PLATFORM_FLAGS=-municode
else
    PLATFORM_FLAGS=
endif

comp:
	g++ $(IMPORTANT_FLAGS) $(PLATFORM_FLAGS) $(FILES) -o ./bin/todo

all:
	g++ $(IMPORTANT_FLAGS) $(EXTRA_FLAGS) $(PLATFORM_FLAGS) $(FILES) $(INCLUDE) -o ./bin/todo

run:
	gcc -c ./include/sqlite3.c -o sqlite3.o
	g++ $(IMPORTANT_FLAGS) $(EXTRA_FLAGS) $(PLATFORM_FLAGS) $(FILES) $(INCLUDE) -o ./bin/todo
