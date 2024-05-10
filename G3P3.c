//Anaphat Sueakhamron 6480228
//Akkhrawin Nair 6580013
//Pakin Panawattanakul 6580043
//Pannawish Kriengyakul 6580117
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

void signal_handler(int SIGNO) { exit(0); } //when parent receive SIGTERM from child 

struct my_msg {
  long msg_type;
  char data[BUFSIZ];
};

int main(int argc, char **argv) {
  int msgID;
  pid_t pid;
  char buffer[BUFSIZ];
  memset(buffer, '\0', BUFSIZ);
  struct my_msg msg;

  if (argc != 2) {
    fprintf(stderr, "Usages %s <[1,2]>\n", *argv);
    exit(EXIT_FAILURE);
  } // check the number of arguments

  argv++;
  if (atoi(*argv) == 1) // process 1
  {
    msgID = msgget((key_t)6580043, 0666 | IPC_CREAT); //create or acess message queue
    pid = fork(); // fork child
    switch (pid) {
    case -1:
      perror("Forking failed\n");
      exit(EXIT_FAILURE);
      break;
    case 0: // child process1 receive msgtype2
      msg.msg_type = 2;
      do {
        msgrcv(msgID, &msg, BUFSIZ,msg.msg_type, 0);//receive msg from ms queue
        printf("%s", msg.data);//print to stdout
      } while (strcmp(msg.data, "end chat\n") != 0);//if send "end chat\n" stop loop//if data receive is "end chat\n" end loop
      pid_t parent = getppid();//get parent pid to sent signal to terminate parent(this case child1 know 
      // parent1 does not know end chat)
      kill(parent, SIGTERM);//sent signal to parent
      msgctl(msgID,IPC_RMID,0);// I let the child process that receive "end chat\n" destory msgq to avoid destroying many times
      break;
    default: // parent process1 send msgtype1
      signal(SIGTERM, signal_handler); // wait for SIGTERM from child in case child receive"end chat\n"
      do {
        fgets(buffer, BUFSIZ, stdin);//get message from stdin to buffer
        strcpy(msg.data, buffer);//copy message to msg.data
        msg.msg_type = 1;//detmermine msgtype parent1 send 1
        msgsnd(msgID, &msg, BUFSIZ, 0);//send msg
      } while (strcmp(msg.data, "end chat\n") != 0);//if send "end chat\n" stop loop
      kill(pid, SIGTERM);//send SIGTERM to child since this case parent know "end chat\n" and child does not
    }
  }

  else if (atoi(*argv) == 2) // process 2
  {
    msgID = msgget((key_t)6580043, 0666 | IPC_CREAT);
    pid = fork();
    switch (pid) {
    case -1:
      perror("Forking failed\n");
      exit(EXIT_FAILURE);
      break;
    case 0: // child process2 receive msg type1
      msg.msg_type = 1;
      do {
        msgrcv(msgID, &msg, BUFSIZ,msg.msg_type, 0);//receive msg from ms queue
        printf("%s", msg.data);//print to stdout
      } while (strcmp(msg.data, "end chat\n") != 0);//if send "end chat\n" stop loop//if data receive is "end chat\n" end loop
      pid_t parent = getppid();//get parent pid to sent signal to terminate parent(this case child2 know 
      // parent2 does not knowend chat)
      kill(parent, SIGTERM);//sent signal to parent
      msgctl(msgID,IPC_RMID,0);// I let the child process that receive "end chat\n" destroy msgq to avoid destroying many times
      break;

    default: // parent process2 send msgtype2
      signal(SIGTERM, signal_handler); // wait for SIGTERM from child in case child receive"end chat\n"
      do {
        fgets(buffer, BUFSIZ, stdin);//get message from stdin to buffer
        strcpy(msg.data, buffer);//copy message to msg.data
        msg.msg_type = 2;//detmermine msgtype parent2 send 2
        msgsnd(msgID, &msg, BUFSIZ, 0);//send msg
      } while (strcmp(msg.data, "end chat\n") != 0);//if send "end chat\n" stop loop
      kill(pid, SIGTERM);//send SIGTERM to child since this case parent know "end chat\n" and child does not
    }
  } 
  else {
    perror("Invalid input(Insert 1 or 2)\n");
    exit(EXIT_FAILURE);
  }
  return 0;
}