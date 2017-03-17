all: run

clean:
	rm *.o

run: build
	./exo2

build: main2.o
	g++ main2.o -lfann -lplplotd -o exo2

main2.o: main2.cpp
	g++ -std=c++11 -O3 -I /usr/include/plplot/ -c main2.cpp