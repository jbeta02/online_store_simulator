#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>
#include "game_summary.h"
#include "customer.h"
#include "customerStruct.h"

int principle;
struct Customer *customers[30];
pthread_mutex_t mutex;

// thread function that manages customer spending
void* cashflow(void *) {

    // clean up buffer
    fflush(stdout);

    int week = 1;
    for (int days = 0; days < 30; days++) {
        if (days % 7 == 0) { // check if week has passed
            if (week >= 4) 
                week = 3; // prevent division by 0
            for (int i = 0; i < sizeof(customers)/sizeof(struct Customer*); i++) {
                if (customers[i] == NULL) // only work with defined Customers
                    break;
                // update customers values based on amount spent
                customers[i]->monthlyIncome = customers[i]->monthlyIncome - customers[i]->dailyFreeCash;
                customers[i]->weeklyfreeCash = customers[i]->weeklyfreeCash - customers[i]->dailyFreeCash;

                customers[i]->weeklyfreeCash = (customers[i]->monthlyIncome) / (4-week)  * getRandom()/10; // out of the weekly cash take a percent to get the free weekly cash
                changePerception(customers[i], customers[i]->brandPerception);
            }
            week++; // count number of weeks that have passed
        }

        // purchase item and update
        for (int i = 0; i < sizeof(customers)/sizeof(struct Customer*); i++) {
            if (customers[i] == NULL)
                break;
            purchase(customers[i]);
        }
        sleep(1); // one day is 1 second
    }
    
    return NULL;
}

// thread function that manages prompt and options selected
void* gamePrompt() {
    int option = 0;
    int adCost = 300;
    int qualityCost = 100;
    // loop prompt
    while (option != 3) {
        printf(
        "\n1) Increase advertisement spending (increase number customers) (-$%d)\n"
        "2) Increase quality of store (increase brand percepton of all customers by 1 grade) (-$%d)\n"
        "3) Leave management (let business self run for rest of month)\n", adCost, qualityCost
        );
        scanf("%d", &option);

        // react to option 1
        int len = sizeof(customers)/sizeof(struct Customer*);
        if (option == 1 && principle > adCost) { // && check left first, if false don't check right just set whole conditional to false
            pthread_mutex_lock(&mutex); // use mutex to prevent modification to priciple and customers while customer is being added
            // subtract from money balance
            principle -= adCost;
            // add customer to end of customer list
            for (int i = 0; i < len; i++) {
                if (customers[i] == NULL){
                    customers[i] = populateCustomer("name", getRandom() * 500, 'D');
                    break;
                }
            }
            pthread_mutex_unlock(&mutex);
        }
        // react to option 2
        if (option == 2 && principle > qualityCost) {
            principle -= qualityCost;
            for (int i = 0; i < len; i++) {
                if (customers[i] == NULL) {
                    break;
                }
                // increase brand perception
                pthread_mutex_lock(&mutex); // use mutex to prevent modification to priciple and customers while brand per is being changed
                incrementPerception(customers[i], 1);
                pthread_mutex_unlock(&mutex);
            }
        }
    }
    printf("Waiting till end of month...\n");
}

int main() { // first last principle

    pthread_mutex_init(&mutex, NULL); // init mutex
    int fd[2]; // file descriptor [0] read, [1] write
    pipe(fd); // int pipe

    int pid = fork();

    customers[0] = populateCustomer("John", 2500, 'F'), 
    customers[1] = populateCustomer("Dave", 3000, 'F');
    customers[2] = populateCustomer("Mike", 1000, 'F');
    customers[3] = populateCustomer("Ash", 2300, 'F');
    customers[4] = populateCustomer("Leo", 1500, 'F');
    
    if (pid == 0) { // check if in child
        close(fd[0]); // close read since won't be reading in this child
        // create threads
        int status;
        pthread_t t1_id, t2_id;
        principle = 1000;
        // create threads
        pthread_create(&t1_id, NULL, cashflow, NULL);
        pthread_create(&t2_id, NULL, gamePrompt, NULL);
        // wait for threads to end
        pthread_join(t1_id, NULL);
        pthread_join(t2_id, NULL);
        printf("in child\n");

        for (int i = 0; i < sizeof(customers)/sizeof(struct Customer*); i++) {
            if (customers[i] == NULL)
                break;
            // display customer list to command line and print to file with linux commands
            display(customers[i]);
            printFile(customers[i]);
        }
        // write and close pipe to send priciple to parent
        write(fd[1], &principle, sizeof(int));
        close(fd[1]);
    }

    wait(NULL); // wait for child to end

    if (pid > 0){ 
        close(fd[1]); // close write of parent pipe
        // print game balance and summary
        printf("in parent\n");
        read(fd[0], &principle, sizeof(int)); // read principle from child
        close(fd[0]);
        printf("principle: %d\n", principle);
        // clear up memory
        pthread_mutex_destroy(&mutex);
        destroyCustomers(); // free heap
    }

    return 0;
}

