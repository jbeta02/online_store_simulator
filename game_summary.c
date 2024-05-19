#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include "online_store_simulation.c"


// display to console
void display() {
    for (int i = 0; i < sizeof(customers)/sizeof(struct Customer*); i++) {
        if (customers[i] == NULL)
            break;
        printf("%s m%d w%d d%d %c\n", customers[i]->name, customers[i]->monthlyIncome, customers[i]->weeklyfreeCash, customers[i]->dailyFreeCash, customers[i]->brandPerception);
    }
}


// print to file
void print() {

}