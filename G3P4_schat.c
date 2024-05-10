//Anaphat Sueakhamron 6480228
//Akkhrawin Nair 6580013
//Pakin Panawattanakul 6580043
//Pannawish Kriengyakul 6580117
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MEM_SIZE 4096
#define BUFSIZE 1024

void signal_handler(int signum){
  exit(0);
}
struct shm_st {
  int flag1; //for parent1 child2
  int flag2; //for parent2 child1
  char data1[BUFSIZE]; //for parent1 child2
  char data2[BUFSIZE]; //for parent2 child1
};

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usages %s <[1,2]>\n", *argv);
    exit(EXIT_FAILURE);
  } //check for invalid input

  int shmID;
  pid_t child;
  char buffer[BUFSIZE] = "";
  void *sh_mem = NULL;
  struct shm_st *shm_area;
  shmID = shmget((key_t)6580043, MEM_SIZE, 0666 | IPC_CREAT);
  if (shmID == -1) {
    fprintf(stderr, "shmget() failed\n");
    exit(EXIT_FAILURE);
  }

  argv++;
  if (atoi(*argv) == 1) // Process1
  {
    child = fork();
    switch (child) {
    case -1:
      perror("Forking failed\n");
      exit(EXIT_FAILURE);
      break;
      
    case 0: // Child1 receive data2 from parent2 and print
      sh_mem = shmat(shmID, NULL, 0);
      if (sh_mem == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
      }
      shm_area = (struct shm_st *)sh_mem;
      shm_area->flag2 = 0;
      do {
        if (shm_area->flag2) {
          printf("%s", shm_area->data2);
          shm_area->flag2 = 0;
        }
        sleep(1);
      } while (strncmp(shm_area->data2, "end chat", 8) != 0);
      printf("end chat\n");
      pid_t parent = getppid();
      kill(parent, SIGTERM);
      break;
      
    default: // Parent1 get data1 from stdin and send to child2
      signal(SIGTERM,signal_handler);
      sh_mem = shmat(shmID, NULL, 0);
      if (sh_mem == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
      }
      shm_area = (struct shm_st *)sh_mem;
      do {
        while (shm_area->flag1) {
          sleep(1);
        }
        fgets(buffer, BUFSIZE, stdin);
        strcpy(shm_area->data1, buffer);
        shm_area->flag1 = 1;

      }while (strncmp(shm_area->data1, "end chat", 8) != 0);
      if (shmdt(sh_mem) == -1 || shmctl(shmID, IPC_RMID, 0) == -1)
      {
        fprintf(stderr, "shmdt or shmctl failed\n");
        exit(EXIT_FAILURE);
      }
      kill(child,SIGTERM);
    }
  }
  if (atoi(*argv) == 2) // Process2
  {
    child = fork();
    switch (child) {
    case -1:
      perror("Forking failed\n");
      exit(EXIT_FAILURE);
      break;

    case 0: // Child2 receive data1 from parent1 and print
      sh_mem = shmat(shmID, NULL, 0);
      if (sh_mem == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
      }
      shm_area = (struct shm_st *)sh_mem;
      shm_area->flag1 = 0;
      do {
        if (shm_area->flag1) {
          printf("%s", shm_area->data1);
          shm_area->flag1 = 0;
        }
        sleep(1);
      } while (strncmp(shm_area->data1, "end chat", 8) !=0);
      pid_t parent = getppid();
      kill(parent, SIGTERM);
      break;
      
    default: // Parent2 get data2 from stdin and send to child1
      signal(SIGTERM,signal_handler);
      sh_mem = shmat(shmID, NULL, 0);
      if (sh_mem == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
      }
      shm_area = (struct shm_st *)sh_mem;
      do
      {
        while (shm_area->flag2) 
        {
          sleep(1);
        }
        fgets(buffer, BUFSIZE, stdin);
        strcpy(shm_area->data2, buffer);
        shm_area->flag2 = 1;
      }while (strncmp(shm_area->data2, "end chat", 8) != 0);
      if (shmdt(sh_mem) == -1 || shmctl(shmID, IPC_RMID, 0) == -1)
      {
        fprintf(stderr, "shmdt or shmctl failed\n");
        exit(EXIT_FAILURE);
      }
      kill(child,SIGTERM);
    }
  }

  exit(EXIT_SUCCESS);
  return 0;
}