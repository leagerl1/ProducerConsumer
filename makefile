FLAGS = -Wall

all: prodcon.o
	gcc $(FLAGS) -o prodcon prodcon.o -lpthread -lrt
prodcon.o: prodcon.c
	gcc $(FLAGS) -c prodcon.c
clean:
	rm -rf *.o prodcon
