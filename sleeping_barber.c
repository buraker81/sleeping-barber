#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>           
#include <unistd.h>
#include <sys/stat.h>  

#define CUSTOMERS 10
#define CHAIRS 3

sem_t* customer_sem;
sem_t* barber_sem;
pthread_mutex_t mutex;

int waiting = 0;
int cut_count = 0;
int skipped_count = 0;

void* fn_barber(void* args);
void* fn_customer(void* args);

int main(){
    /*not setting the seed*/
    srand(time(NULL));

    /*initializing the semaphores and mutexes*/
    pthread_t btid;
    pthread_t ctid[CUSTOMERS];

    customer_sem = sem_open("/customer_sem", O_CREAT | O_EXCL, 0644, 0);
    barber_sem = sem_open("/barber_sem", O_CREAT | O_EXCL, 0644, 0);

    if (customer_sem == SEM_FAILED || barber_sem == SEM_FAILED) {
        sem_unlink("/customer_sem");
        sem_unlink("/barber_sem");
        customer_sem = sem_open("/customer_sem", O_CREAT, 0644, 0);
        barber_sem = sem_open("/barber_sem", O_CREAT, 0644, 0);
    }

    pthread_mutex_init(&mutex, NULL);

    /*creating barber process*/
    pthread_create(&btid, NULL, fn_barber, NULL);
    printf("\033[1;34mBarber entered his shop\033[0m\n");

    /* creating customer processes at random intervals */
    for (int i = 0; i < CUSTOMERS; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&ctid[i], NULL, fn_customer, id);
        sleep(rand() % 3 + 1); 
    }

    /*waiting threads finish it execution */
    for (int i = 0; i < CUSTOMERS; i++) {
        pthread_join(ctid[i], NULL);
    }

    /*to exit the barber from the shop, last to avoid infinite waiting due to bounded # of customers*/
    sem_post(customer_sem); 
    //wait also the barber
    pthread_join(btid, NULL); 
    

    //close the semaphroes and kill the mutex
    sem_close(customer_sem);
    sem_unlink("/customer_sem");
    sem_close(barber_sem);
    sem_unlink("/barber_sem");

    pthread_mutex_destroy(&mutex);

    printf("\033[1;34m\nAll customers have been handled.\033[0m\n");
    printf("Haircuts: %d | Skipped: %d\n", cut_count, skipped_count);

    return 0;
}

void* fn_barber(void* args){
    while (1) {
        //wait for a customer
        sem_wait(customer_sem); 
        //enter critical section
        pthread_mutex_lock(&mutex);
        if (cut_count + skipped_count >= CUSTOMERS) {
            pthread_mutex_unlock(&mutex);
            break; // break the loop
        }
        waiting--;
        pthread_mutex_unlock(&mutex);

        usleep(10000); // to avoid printing confliction
        printf("Barber started cutting\n");
        sleep(rand() % 4 + 2); // each cut has own time interval 
        sem_post(barber_sem); // signal the barber
    }
    return NULL;
}

void* fn_customer(void* args){
    int id = *(int*) args;   
    free(args);
    //enter critical section
    pthread_mutex_lock(&mutex);
    if(waiting < CHAIRS){
        //increment the number of waiting people
        waiting++;
        pthread_mutex_unlock(&mutex);

        //signal customer to wake up the barber
        sem_post(customer_sem);
        printf("Customer %d is waiting. Waiting count: %d\n", id, waiting);
       
        //wait barber to finish his job
        sem_wait(barber_sem);
        printf("\033[1;32mCustomer %d is got a haircut.\033[0m\n", id);
       
        //increment cut count and release the lock;
        pthread_mutex_lock(&mutex);
        cut_count++;
        pthread_mutex_unlock(&mutex);
    } else {
        printf("\033[1;31mCustomer %d left, no available chairs.\033[0m\n", id);
        skipped_count++; // increment the # of customers that could not get a cut
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}
