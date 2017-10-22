// Jonathan Jeans
// 10/21/2017
// OS - Cenek
// Dining Philosopher problem

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>

#define LEFT ((ph_num+count-1)%count)     // number of i’s left neighbor
#define RIGHT ((ph_num+1)%count)          // number of i’s right neighbor
#define THINKING 0                        // philosopher is thinking
#define HUNGRY 1                          // philosopher is trying to get forks
#define EATING 2                          // philosopher is eating

sem_t mutex;                              // mutual exclusion for critical regions
sem_t *s;                                 // one semaphore per philosopher

void * philosopher(void *num);
void take_forks(int);
void put_forks(int);
void test(int);
bool is_number(char*);

int *state;                        // array to keep track of everyone’s state
int *phil;                         // array to hold list of philosophers
int count;                         // number of philosophers
int knives;                        // number of knives
int sleep_time;                    // amount of time a philosopher sleeps
bool original = true;

int main(int argc, char **argv)
{
    if (argc != 4){                 // Error checking for number of inputs
        perror("Bad input");
        exit(1);
    }
    if (!is_number(argv[1])){       // Error Checking the input for philosophers
        perror("Bad input");
        exit(1);
    }
    if (!is_number(argv[2])){       // Error checking the input for knives
        perror("Bad input");
        exit(1);
    }
    if (!is_number(argv[3])){       // Error checking for input of time
        perror("Bad input");
        exit(1);
    }

    count = atoi(argv[1]);          // initial number of philosophers
    knives = atoi(argv[2]);         // initial number of knives
    sleep_time = atoi(argv[3]);     // time for sleeping

    if (knives > 0){
        original = false;           // if knives = 0, then run original code w/o knives
    }

    s = malloc(count *sizeof(s[0]));
    state = malloc(count *sizeof(state[0]));
    phil = malloc(count *sizeof(phil[0]));

    for(int i=0; i<count; i++)           // initialize philosophers
        phil[i] = i;
    pthread_t thread_id[count];
    sem_init(&mutex,0,1);               // initialize mutex semaphore
    for(int i=0; i<count; i++)
        sem_init(&s[i],0,0);            // initialize philosopher semaphores
    for(int i=0; i<count; i++)
    {
        pthread_create(&thread_id[i], NULL, philosopher, &phil[i]);
        printf("Philosopher %d is thinking\n",i+1);
    }
    for(int i=0; i<count; i++)
        pthread_join(thread_id[i],NULL);
}

bool is_number(char number[])           // used to error check user input
{
    for (int i=0; number[i] != 0; i++){ // runs through each character in array
        if (!isdigit(number[i]))        // and determines if it's a number
            return false;
    }
    return true;
}

void sig_handler(int signo)                 // function used for ctrl-c signal
{
    if (signo == SIGINT){                   // function cleans up all semaphores
        printf("\nReceived SIGINT ctr-c\n");
        sem_close(&mutex);                  // close mutex
        printf("mutex closed...\n");
        for (int i=0; i<count; i++){
            sem_close(&s[i]);               // close all philosophers
            printf("Philosopher %d closed...\n", i+1);
        }
        printf("Exiting program...\n");
        exit(1);
    }
}

void *philosopher(void *num)
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = sig_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);    // used when ctr+c signal found

    while(1)                        // repeat forever
    {
        int *i = num;
        sleep(sleep_time + 1);
        take_forks(*i);             // acquire two forks or block
        sleep(sleep_time);
        put_forks(*i);              // put both forks back on table
    }
}

void take_forks(int ph_num)
{
    sem_wait(&mutex);               // enter critical region
    state[ph_num] = HUNGRY;         // record fact that philosopher i is hungry
    printf("Philosopher %d is Hungry\n",ph_num+1);
    test(ph_num);                   // try to acquire 2 forks
    sem_post(&mutex);               // exit critical region
    sem_wait(&s[ph_num]);           // block if forks were not acquired
    sleep(sleep_time + 1);
}

void put_forks(int ph_num)
{
    sem_wait(&mutex);               // enter critical region
    state[ph_num] = THINKING;       // philosopher has finished eating
    knives = knives + 1;
    printf("Philosopher %d putting fork %d and %d down\n",ph_num+1,LEFT+1,ph_num+1);
    if (!original)
        printf("Philosopher %d putting knife down\n", ph_num+1);
    printf("Philosopher %d is thinking\n",ph_num+1);
    test(LEFT);                     // see if left neighbor can now eat
    test(RIGHT);                    // see if right neighbor can now eat
    sem_post(&mutex);               // exit critical region
}

void test(int ph_num)
{
    if(original){       // use this condition if running original program
        if (state[ph_num] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
        {
            state[ph_num] = EATING;
            sleep(sleep_time + 2);
            printf("Philosopher %d takes fork %d and %d\n",ph_num+1,LEFT+1,ph_num+1);
            printf("Philosopher %d is Eating\n",ph_num+1);
            sem_post(&s[ph_num]);
        }
    }
    else{               // use this condition if running with knives
        if (state[ph_num] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING
            && knives > 0)
        {
            knives = knives - 1;
            state[ph_num] = EATING;
            sleep(sleep_time + 2);
            printf("Philosopher %d takes fork %d and %d\n",ph_num+1,LEFT+1,ph_num+1);
            printf("Philosopher %d takes knife from pool\n", ph_num+1);
            printf("Philosopher %d is Eating\n",ph_num+1);
            sem_post(&s[ph_num]);
        }
    }
}
