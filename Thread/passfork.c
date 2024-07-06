#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

/*
  S: semaphore
    amounts of available resources (S > 0)

  P(S) {
    while(s <= 0) {
      do nothing;
    }
    S--;
  } 

  V(S) {
    S++;
  } 
*/

sem_t roads;

void* cars(void* args) {
  printf("(%lu) I intended to pass the fork!\n", pthread_self() % 100);

  sem_wait(&roads);

  printf("(%lu) I am AT the road!\n", pthread_self() % 100);
  sleep(1);
  printf("(%lu) I have passed the road!\n", pthread_self() % 100);
  sleep(1);

  sem_post(&roads);
  pthread_exit(NULL);
}

int main() {
  pthread_t tid[5];
  sem_init(&roads, 0, 2);
  for (int i = 0; i < 5; i++) {
    pthread_create(tid + i, NULL, cars, NULL);
  }
  for (int i = 0; i < 5; i++) {
    pthread_join(tid[i], NULL);
  }
  return 0;
}