#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 10

typedef struct BufferQueue {
  int buffer[BUFSIZE];
  int front;
  int rear;
} BufferQueue;

typedef struct Arg {
  sem_t mutex, empty, full;
  BufferQueue buf;
} Arg;

void InitArg(Arg* arg) {
  sem_init(&(arg->mutex), 0, 1);
  sem_init(&(arg->empty), 0, BUFSIZE);
  sem_init(&(arg->full), 0, 0);
  arg->buf.front = 0;
  arg->buf.rear = 0;
}

void *Producer(void *args) { 
  Arg* arg = (Arg*) args;
  for (int i = 0; i < BUFSIZE * 2; i++) {
    sem_wait(&(arg->empty));  // P(empty)
    sem_wait(&(arg->mutex));  // P(mutex)
    printf("Producer add %d to Buffer\n", i + 1);
    arg->buf.buffer[arg->buf.rear] = i + 1;
    arg->buf.rear = (arg->buf.rear + 1) % BUFSIZE;
    sem_post(&(arg->mutex)); // V(mutex)
    sem_post(&(arg->full)); // V(full)
  }
  pthread_exit(NULL);
}

void *Consumer(void *args) { 
  Arg* arg = (Arg*) args;
  for (int i = 0; i < BUFSIZE * 2; i++) {
    sem_wait(&(arg->full)); // P(full)
    sem_wait(&(arg->mutex)); // P(mutex)
    printf("\t\t\t  Consumer remove %d from buffer\n", arg->buf.buffer[arg->buf.front]);
    arg->buf.front = (arg->buf.front + 1) % BUFSIZE;
    sem_post(&(arg->mutex)); // V(mutex)
    sem_post(&(arg->empty)); // V(empty)
  }
  pthread_exit(NULL); 
}

int main() {
  Arg arg;
  pthread_t Producer_t, Consumer_t;

  InitArg(&arg);
  pthread_create(&Producer_t, NULL, Producer, &arg);
  pthread_create(&Consumer_t, NULL, Consumer, &arg);

  pthread_join(Producer_t, NULL);
  pthread_join(Consumer_t, NULL);

  sem_destroy(&arg.mutex);
  sem_destroy(&arg.empty);
  sem_destroy(&arg.full);
  return 0;
}