// Compile with "g++ dining.cpp -o dining -pthread"
// Execute program with "./dining"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace ::std;

// Function Declerations
void *dining(void *num);
void pickup_forks(int philosopher_id);
void return_forks(int philosopher_id);
void thinking(int philosopher_id);
void eating(int philosopher_id);

// Represents the 3 states philosophers can be in
enum philosophers_state
{
    THINKING,
    HUNGRY,
    EATING
};
enum philosophers_state state[5]; // Stores states
int philosopher_ids[5] = {1, 2, 3, 4, 5};
pthread_t philosophers[5];
pthread_mutex_t lock; // Mutex lock for critical section
pthread_cond_t conditions[5];

int main()
{
    // Initializing mutex lock
    pthread_mutex_init(&lock, NULL);

    // Initializing condition variables
    for (int i = 0; i < 5; i++)
    {
        pthread_cond_init(&conditions[i], NULL);
    }

    // Creating threads for each philosophers
    for (int i = 0; i < 5; i++)
    {
        pthread_create(&philosophers[i], NULL, dining, &philosopher_ids[i]);
    }

    // Join philosophers threads (waits for threads to terminate)
    for (int i = 0; i < 5; i++)
    {
        pthread_join(philosophers[i], NULL);
    }

    // Destroy mutex lock and condition variables
    pthread_mutex_destroy(&lock);
    for (int i = 0; i < 5; i++)
    {
        pthread_cond_destroy(&conditions[i]);
    }
}

// Let philosopher eat if left philosopher and right philosopher are not eating
// Use conditional variable to stall until both adjacent forks are present
void pickup_forks(int philosopher_id)
{
    int left = ((philosopher_id - 1) + 5) % 5;
    int right = (philosopher_id + 1) % 5;
    pthread_mutex_lock(&lock);
    state[philosopher_id] = HUNGRY;

    while ((state[philosopher_id] == HUNGRY) && (state[left] == EATING || state[right] == EATING))
    {
        pthread_cond_wait(&conditions[philosopher_id], &lock);
    }
    state[philosopher_id] = EATING;
    printf("Forks are with philosopher #%d\n", philosopher_id);
    pthread_mutex_unlock(&lock);
}

// Philosopher is done eating, sets forks down, begins thinking, and allow other philosophers to eat
// Signals the conditional variables to see if left & right philosophers are able to eat (if present with both forks)
void return_forks(int philosopher_id)
{
    int left = ((philosopher_id - 1) + 5) % 5;
    int right = (philosopher_id + 1) % 5;
    pthread_mutex_lock(&lock);
    state[philosopher_id] = THINKING;

    pthread_cond_signal(&conditions[left]);
    // printf("Fork #%d with %d\n", left, philosopher_id);
    pthread_cond_signal(&conditions[right]);
    // printf("Fork #%d with %d\n", right, philosopher_id);
    pthread_mutex_unlock(&lock);
}

// Philosopher is thinking
void thinking(int philosopher_id)
{
    srand(time(NULL));
    int thinkTime = ((rand() % 3) + 1);
    printf("Philosopher #%d took %d seconds thinking\n", philosopher_id, thinkTime);
    sleep(thinkTime);
}

// Philosopher is eating
void eating(int philosopher_id)
{
    srand(time(NULL));
    int eatingTime = ((rand() % 3) + 1);
    printf("Philosopher #%d took %d seconds eating\n", philosopher_id, eatingTime);
    sleep(eatingTime);
}

// Main program that runs each philosopher threads
void *dining(void *num)
{
    int id = *((int *)num);
    while (1)
    {
        thinking(id);
        pickup_forks(id);
        eating(id);
        return_forks(id);
    }
}