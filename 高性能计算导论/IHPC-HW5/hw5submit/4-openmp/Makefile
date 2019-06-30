target : main.o
	gcc -o target main.o -fopenmp -O3

main.o : main.c gen.h
	gcc -c main.c -fopenmp -O3

clean :
	rm target main.o