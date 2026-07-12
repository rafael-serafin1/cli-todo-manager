comp: 
	g++ main.c -o todo
	./todo help

expd:
	g++ -E main.c -o expanded.i