main: online_store_simulation.o customer.o game_summary.o
	gcc -o main online_store_simulation.o customer.o game_summary.o -lm

online_store_simulation.o: online_store_simulation.c customer.c customer.h game_summary.c game_summary.h customerStruct.h
	gcc -c online_store_simulation.c -lpthread

customer.o: customer.c customer.h customerStruct.h
	gcc -c customer.c -lpthread

game_summary.o: game_summary.c game_summary.h customerStruct.h
	gcc -c game_summary.c