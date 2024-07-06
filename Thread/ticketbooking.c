#include <stdio.h>
#include <pthread.h>
#include <time.h>

int ticketAmount = 2;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* ticketAgent(void* arg) {
  // acquire lock
  pthread_mutex_lock(&lock);

  int t = ticketAmount;
  if (t > 0) {
    printf("One ticket sold!\n");
    t--;
  } else {
    printf("Tickets sold out!\n");
  }
  ticketAmount = t;

  //release lock
  pthread_mutex_unlock(&lock);

  pthread_exit(NULL);
}

int main() {
  pthread_t ticketAgents_tid[2];

  for (int i = 0; i < 2; i++) {
    pthread_create(ticketAgents_tid + i, NULL, ticketAgent, NULL);
  }

  for (int i = 0; i < 2; i++) {
    pthread_join(ticketAgents_tid[i], NULL);
  }

  printf("The left tickets is %d\n", ticketAmount);
  return 0;
}
