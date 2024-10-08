#define _GNU_SOURCE

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
  Strategy:
    When the file is being written by a writer, any others can't use it;
    When the file is being read by a reader, other readers can read it but any writers can't use it.
*/

#define READERS_NUM 10 
#define WRITERS_NUM 5 

typedef struct {
  sem_t file, mutex_reader_count;
  int reader_count;
} Arg;

void init(Arg* arg) {
  sem_init(&(arg->file), 0, 1);
  sem_init(&(arg->mutex_reader_count), 0, 1);
  arg->reader_count = 0;
}

void* Writer(void *args) {
  Arg* arg = (Arg*) args;
  sem_wait(&(arg->file));
  printf("Writer %lu is writing!\n", pthread_self() % 100);
  usleep(500000);
  printf("Writer %lu has finished writing!\n", pthread_self() % 100);
  sem_post(&(arg->file));

  pthread_exit(NULL);
}

void* Reader(void* args) {
  Arg* arg = (Arg*) args;
  sem_wait(&(arg->mutex_reader_count));
  if (arg->reader_count == 0) {
    sem_wait(&(arg->file));
  }
  arg->reader_count++;
  sem_post(&(arg->mutex_reader_count));

  printf("\t\t\tReader %lu is reading!\n", pthread_self() % 100);
  usleep(500);
  printf("\t\t\tReader %lu has finished reading!\n", pthread_self() % 100);
  usleep(1000);
  
  sem_wait(&(arg->mutex_reader_count));
  arg->reader_count--;
  if (arg->reader_count == 0) {
    sem_post(&(arg->file));
  }
  sem_post(&(arg->mutex_reader_count));

  pthread_exit(NULL);
}

int main() {
  Arg arg;
  init(&arg);

  pthread_t writers_id[WRITERS_NUM];
  pthread_t readers_id[READERS_NUM];

  for (int i = 0; i < WRITERS_NUM; i++) {
    pthread_create(writers_id + i, NULL, Writer, &arg);
  }
  for (int i = 0; i < READERS_NUM; i++) {
    pthread_create(readers_id + i, NULL, Reader, &arg);
  }

  for (int i = 0; i < WRITERS_NUM; i++) {
    pthread_join(writers_id[i], NULL);
  }
  for (int i = 0; i < READERS_NUM; i++) {
    pthread_join(readers_id[i], NULL);
  }
  return 0;
}
