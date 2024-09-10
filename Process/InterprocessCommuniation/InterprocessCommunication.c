#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define MSGSIZE 100
#define SHAREDMEMSIZE 2048
#define TEXTSIZE 512

typedef struct MsgBuf {
  long int msgtype;       // 消息类型
  char anytext[TEXTSIZE]; // 消息数据
} MsgBuf;

void pip() {
  int pip_fd[2];
  if (pipe(pip_fd) == -1) {
    printf("Pipe creation failed!");
  }

  pid_t child_pid = fork(); // 创建子进程
  if (child_pid == -1) {
    printf("Fork failed\n");
    exit(EXIT_FAILURE);
  } else if (child_pid == 0) {
    close(pip_fd[1]);
    char buffer[MSGSIZE];
    for (int i = 0; i < 10; i++) {
      read(pip_fd[0], buffer, sizeof(buffer));
      printf("子进程在管道中读取数据:");
      printf("%s\n", buffer);
    }
    close(pip_fd[0]);
  } else {
    close(pip_fd[0]);
    char message[MSGSIZE] = "A";
    for (int i = 0; i < 10; i++) {
      message[0] = 'A' + i;
      printf("父进程在管道中写入数据:%c\n", message[0]);
      usleep(100000);
      write(pip_fd[1], message, sizeof(message));
    }
    close(pip_fd[1]);
  }

  if (child_pid == 0) {
    _exit(0);
  } else {
    waitpid(child_pid, NULL, 0);
  }
}

void SharedMemory() {
  key_t semkey = ftok("./temp", 1);
  if (semkey == -1) {
    printf("ftok failed\n");
    exit(EXIT_FAILURE);
  }

  int sharedMemId = shmget(semkey, SHAREDMEMSIZE, 0666 | IPC_CREAT);
  // 创建共享内存，获取共享内存标识符

  if (sharedMemId == -1) {
    printf("Create shared memory failed\n");
    exit(EXIT_FAILURE);
  }
  char *data = (char *)shmat(sharedMemId, NULL, 0);
  int index = 0;
  for (char c = 'A'; c <= 'G'; c++) {
    printf("父进程写入数据:%c\n", c);
    data[index++] = c;
    data[index] = 0;
  }
  if (shmdt(data) == -1) {
    printf("detach error\n");
  }

  pid_t child_pid = fork(); // 创建子进程

  if (child_pid == -1) {
    printf("Folk failed");
    exit(EXIT_FAILURE);
  } else if (child_pid == 0) {
    key_t semkey2 = ftok("./temp", 1);
    if (semkey2 == -1) {
      printf("ftok failed\n");
      exit(EXIT_FAILURE);
    }

    int sharedMemId2 = shmget(semkey2, SHAREDMEMSIZE, 0666 | IPC_CREAT);
    // 创建共享内存，获取共享内存标识符

    if (sharedMemId2 == -1) {
      printf("Create shared memory failed\n");
      exit(EXIT_FAILURE);
    }
    // waitpid(child_pid, NULL, 0);

    char *data = (char *)shmat(sharedMemId2, NULL, 0);
    printf("子进程读出数据:%s", data);
    if (shmdt(data) == -1) {
      printf("detach error\n");
    }
  }

  if (child_pid > 0) {
    waitpid(child_pid, NULL, 0);
  } else {
    exit(0);
  }
}

void MessageQueue() {
  int msgid = msgget((key_t)1000, 0664 | IPC_CREAT);
  // 父进程创建发送消息队列

  if (msgid == -1) {
    printf("Msgget failed\n");
    exit(EXIT_FAILURE);
  }

  pid_t child_pid = fork();
  // 创建子进程
  long int msg_to_rcv = 0;

  if (child_pid == -1) {
    printf("Folk failed\n");
    exit(EXIT_FAILURE);
  } else if (child_pid == 0) {
    int msgid2 = msgget((key_t)1000, 0664 | IPC_CREAT);
    // 子进程创建接受消息队列

    if (msgid2 == -1) {
      printf("Msgget2 failed\n");
      exit(EXIT_FAILURE);
    }

    MsgBuf msgBuf;
    for (int i = 0; i < 4; i++) {
      if (msgrcv(msgid2, (void *)&msgBuf, BUFSIZ, msg_to_rcv, 0) == -1) {
        printf("Receiving message failed\n");
        exit(EXIT_FAILURE);
      }
      printf("接受类型为:%ld, 接受内容为: %s\n", msgBuf.msgtype,
             msgBuf.anytext);
      usleep(500000);
    }

    if (msgctl(msgid, IPC_RMID, 0) == -1) {
      printf("删除消息队列失败\n");
      exit(EXIT_FAILURE);
    }
  } else if (child_pid > 0) {
    char typeText[4][10] = {"C++", "Java", "Python", "Rust"};
    MsgBuf msgbuf;
    for (int i = 0; i < 4; i++) {
      msgbuf.msgtype = i + 1;
      strcpy(msgbuf.anytext, typeText[i]);
      if (msgsnd(msgid, (void *)&msgbuf, sizeof(msgbuf), 0) == -1) { // 发送消息
        printf("Sending message failed\n");
        exit(EXIT_FAILURE);
      }
      printf("发送类型为%ld, 发送内容为: %s\n", msgbuf.msgtype, typeText[i]);
      usleep(500000);
    }
  }

  if (child_pid == 0) {
    _exit(0);
  } else {
    waitpid(child_pid, NULL, 0);
  }
}

void menu() {
  printf("\n\n");
  printf("--------------------------\n");
  printf("\t1 共享存储\n");
  printf("\t2 消息队列\n");
  printf("\t3 管道通信\n");
  printf("\t0 退出\n");
  printf("--------------------------\n");
  printf("\n\n");
}

int main() {

  while (1) {
    menu();
    int num;
    scanf("%d", &num);
    if (num == 1) {
      SharedMemory();
    } else if (num == 2) {
      MessageQueue();
    } else if (num == 3) {
      pip();
    } else {
      break;
    }
  }
  return 0;
}
