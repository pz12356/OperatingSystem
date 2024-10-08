#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
  Strategy:
    A philosopher will be dinning when both chopsticks beside him are not being used.
*/

#define PHILOSOPHER_NUM 10

typedef struct {
  sem_t *chopsticks;
  sem_t *mutex;
  int philo_id;
} Arg;

void *Philosopher(void *args) {
  Arg *arg = (Arg *)args;
  sem_wait(arg->mutex + arg->philo_id);
  sem_wait(arg->chopsticks + arg->philo_id);
  sem_wait(arg->chopsticks + (arg->philo_id + 1) % PHILOSOPHER_NUM);
  sem_post(arg->mutex + arg->philo_id);

  printf("Philosopher %d is eating\n", arg->philo_id + 1);
  printf("Philosopher %d has finished eating\n", arg->philo_id + 1);

  sem_post(arg->chopsticks + arg->philo_id);
  sem_post(arg->chopsticks + (arg->philo_id + 1) % PHILOSOPHER_NUM);

  pthread_exit(NULL);
}

int main() {
  Arg args[PHILOSOPHER_NUM];
  sem_t chopsticks[PHILOSOPHER_NUM];
  sem_t mutex[PHILOSOPHER_NUM];

  for (int i = 0; i < PHILOSOPHER_NUM; i++) {
    sem_init(chopsticks + i, 0, 1);
    sem_init(mutex + i, 0, 1);
    args[i].chopsticks = chopsticks;
    args[i].mutex = mutex;
    args[i].philo_id = i;
  }

  pthread_t philosophers[PHILOSOPHER_NUM];
  for (int i = 0; i < PHILOSOPHER_NUM; i++) {
    pthread_create(philosophers + i, NULL, Philosopher, args + i);
  }
  for (int i = 0; i < PHILOSOPHER_NUM; i++) {
    pthread_join(philosophers[i], NULL);
  }
  return 0;
}