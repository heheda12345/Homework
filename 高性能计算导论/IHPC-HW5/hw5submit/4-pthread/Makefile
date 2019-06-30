target : main.o
	gcc -o target main.o -lpthread -O3

main.o : main.c gen.h
	gcc -c main.c -lpthread -O3

clean :
	rm target main.o