FILES=index.cpp parse/parse.cpp commands/commands.cpp config/config.cpp file/file.cpp
IMPORTANT_FLAGS=-std=c++20 -O2
EXTRA_FLAGS=-Wall

comp:
	g++ $(IMPORTANT_FLAGS) $(FILES) -o ./bin/todo

all:
	g++ $(IMPORTANT_FLAGS) $(EXTRA_FLAGS) $(FILES) -o ./bin/todo
