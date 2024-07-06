#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

const int MAX = 3;

int main() {
  pid_t pid = getpid();
  printf("Before fork Process id is %d\n", pid);

  /*
    fork() : Create a new child process
    父进程返回值为子进程id
    子进程返回0
    创建失败返回-1
  */

  pid_t cid = fork();
  int value = 0;
  
  if (cid == 0) {
    // sleep(3);
    printf("Child process id (my parent pid is %d ) : %d\n", getppid(), getpid());
    for (int i = 0; i < MAX; i++) {
      printf("Hello(%d)\n", value--);
    }
  } else {
    printf("Parent process id is %d\n", getpid());
    for (int i = 0; i < MAX; i++) {
      printf("World(%d)\n", value++);
    }

    wait(NULL);
    /*
      wait() 父进程等待子进程结束后再执行

      孤儿进程被系统进程托管
    */
  }

  pause();
  return 0;
}