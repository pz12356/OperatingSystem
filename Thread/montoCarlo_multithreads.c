#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

void* calculate_pi(void* args) {
  unsigned int seed = time(NULL);
  int circle_points = 0;
  int square_points = 0;

  int intervals = *(int*) args;
  for (int i = 0; i < intervals * intervals; i++) {
    double rand_x = 1. * rand_r(&seed) / RAND_MAX;
    double rand_y = 1. * rand_r(&seed) / RAND_MAX;

    circle_points += (rand_x * rand_x + rand_y * rand_y) <= 1;
    square_points++;
  }

  double pi = 4.0 * circle_points / square_points;

  printf("The estimate PI is %lf in %d times\n", pi, intervals * intervals);

  pthread_exit(NULL);
}

int main() {
  clock_t start, delta;
  
  start = clock();
  pthread_t calculate_pi_threads[10]; // tid array
  int args[10]; // thraead parameters array

  // create multiple threads thrick
  for (int i = 0; i < 10; i++) {
    args[i] = 1000 * (i + 1);
    pthread_create(calculate_pi_threads + i, NULL, calculate_pi, args + i);
  }

  for (int i = 0; i < 10; i++) {
    pthread_join(calculate_pi_threads[i], NULL);
  }
  
  delta = clock() - start;
  printf("The time taken in total : %lf seconds\n", 1. * delta / CLOCKS_PER_SEC);
  return 0;
}