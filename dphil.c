// Jonathan Jeans
// 10/21/2017
// OS - Cenek
// Dining Philosopher problem

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>

#define LEFT ((ph_num+count-1)%count)       // number of i’s left neighbor
#define RIGHT ((ph_num+1)%count)          // number of i’s right neighbor
#define THINKING 0                  // philosopher is thinking
#define HUNGRY 1                    // philosopher is trying to get forks
#define EATING 2                    // philosopher is eating

sem_t mutex;                        // mutual exclusion for critical regions
sem_t *s;                           // one semaphore per philosopher

void * philosopher(void *num);
void take_forks(int);
void put_forks(int);
void test(int);

int *state;                        // array to keep track of everyone’s state
int *phil;                         // array to hold list of philosophers
int count;                         // number of philosophers
int knives;                        // number of knives
int sleep_time;                    // amount of time a philosopher sleeps

int main(int argc, char **argv)
{
    count = atoi(argv[1]);
    knives = atoi(argv[2]);
    sleep_time = atoi(argv[3]);

    s = malloc(count *sizeof(s[0]));
    state = malloc(count *sizeof(state[0]));
    phil = malloc(count *sizeof(phil[0]));

    for(int i=0; i<count; i++)          // initialize philosophers
        phil[i] = i;
    pthread_t thread_id[count];
    sem_init(&mutex,0,1);           // initialize mutex semaphore
    for(int i=0; i<count; i++)
        sem_init(&s[i],0,0);        // initialize philosopher semaphores
    for(int i=0; i<count; i++)
    {
        pthread_create(&thread_id[i], NULL, philosopher, &phil[i]);
        printf("Philosopher %d is thinking\n",i+1);
    }
    for(int i=0;i<count;i++)
        pthread_join(thread_id[i],NULL);
}

void *philosopher(void *num)
{
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
    printf("Philosopher %d putting fork %d and %d down\n",ph_num+1,LEFT+1,ph_num+1);
    printf("Philosopher %d is thinking\n",ph_num+1);
    test(LEFT);                     // see if left neighbor can now eat
    test(RIGHT);                    // see if right neighbor can now eat
    sem_post(&mutex);               // exit critical region
}

void test(int ph_num)
{
    if (state[ph_num] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    {
        state[ph_num] = EATING;
        sleep(sleep_time + 2);
        printf("Philosopher %d takes fork %d and %d\n",ph_num+1,LEFT+1,ph_num+1);
        printf("Philosopher %d is Eating\n",ph_num+1);
        sem_post(&s[ph_num]);
    }
}
