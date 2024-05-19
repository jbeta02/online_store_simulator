main: online_store_simulation.o customer.o display.o
	gcc -o main online_store_simulation.o customer.o display.o -lm

online_store_simulation.o: online_store_simulation.c
	gcc -c online_store_simulation.c -lpthread

customer.o: customer.c
	gcc -c customer.c -lpthread

display.o: display.c display.h
	gcc -c display.c

