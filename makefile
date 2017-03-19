TARGETS = main2.cpp TestPool.cpp NeuralNetwork.cpp VariationResult.cpp FileUtils.cpp Plotting.cpp

all: run

clean:
	rm *.o

view:
	python ./viz/server.py

run: build
	./exo2 --input android-features.data
	cp result.data viz

build: main2.o TestPool.o NeuralNetwork.o VariationResult.o FileUtils.o Plotting.o
	g++ -std=c++11 main2.o TestPool.o NeuralNetwork.o VariationResult.o FileUtils.o Plotting.o -lfann -lplplotd -o exo2

main2.o: $(TARGETS)
	g++ -std=c++11 -O3 -I /usr/include/plplot/ -c $(TARGETS)

