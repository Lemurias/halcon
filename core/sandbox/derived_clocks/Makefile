output: main.o
	g++ main.o -o output.bin

main.o: main.cpp *.hpp
	g++ -O3 -c -std=c++20 main.cpp

clean:
	rm *.o output.bin

run:
	./output.bin