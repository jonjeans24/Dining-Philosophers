#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define N 5                         // number of philosophers
#define LEFT (ph_num+(N-1))%N       // number of i’s left neighbor
#define RIGHT (ph_num+1)%N          // number of i’s right neighbor
#define THINKING 0                  // philosopher is thinking
#define HUNGRY 1                    // philosopher is trying to get forks
#define EATING 2                    // philosopher is eating
#define TIME 0

sem_t mutex;                        // mutual exclusion for critical regions
sem_t s[N];                         // one semaphore per philosopher

void * philospher(void *num);
void take_forks(int);
void put_forks(int);
void test(int);

int state[N];                       // array to keep track of everyone’s state
int phil[N];                        // array to hold list of philosophers

int main(int argc, char **argv)
{
    for(int i=0; i<N; i++)          // initialize philosophers
        phil[i] = i;
    pthread_t thread_id[N];
    sem_init(&mutex,0,1);
    for(int i=0; i<N; i++)
        sem_init(&s[i],0,0);
    for(int i=0; i<N; i++)
    {
        pthread_create(&thread_id[i], NULL, philospher, &phil[i]);
        printf("Philosopher %d is thinking\n",i+1);
    }
    for(int i=0;i<N;i++)
        pthread_join(thread_id[i],NULL);
}

void *philospher(void *num)
{
    while(1)                        // repeat forever
    {
        int *i = num;
        sleep(TIME + 1);
        take_forks(*i);             // acquire two forks or block
        sleep(TIME);
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
    sleep(TIME + 1);
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
        sleep(TIME + 2);
        printf("Philosopher %d takes fork %d and %d\n",ph_num+1,LEFT+1,ph_num+1);
        printf("Philosopher %d is Eating\n",ph_num+1);
        sem_post(&s[ph_num]);
    }
}
