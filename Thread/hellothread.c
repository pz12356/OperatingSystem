#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


const int MAX = 3;
int value = 0; // Shared data section belongs to process

void* threadFun(void* arg) {
  printf("In new thread\n");
}

void* hello(void* arg) {
  for (int i = 0; i < MAX; i++) {
    printf("Hello(%d)\n", value++);
  }
}

void* world(void* arg) {
  for (int i = 0; i < MAX; i++) {
    printf("world(%d)\n", value++);
  }
}

int main() {
  // thread id
  pthread_t tid1, tid2;

  /*
    create a thread
  
    Parameters:
      1. thread id address
      2. thread attibute address
      3. thread function address
      4. thread parameters address
  */ 
  pthread_create(&tid1, NULL, hello, NULL);
  pthread_create(&tid2, NULL, world, NULL);

  // 等待指定线程结束
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  printf("In main thread value is %d\n", value);
  return 0;
}