#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "customerStruct.h"

// // display to console
void display(struct Customer* customer) {
    printf("%s m%d w%d d%d %c\n", customer->name, customer->monthlyIncome, customer->weeklyfreeCash, customer->dailyFreeCash, customer->brandPerception);
}


// print to file using linux commands
void printFile(struct Customer* customer) {
    char command[50];
    sprintf(command, "echo \"%s m%d w%d d%d %c \" >> customer_list.txt", customer->name, customer->monthlyIncome, customer->weeklyfreeCash, customer->dailyFreeCash, customer->brandPerception);

    system(command);

}