//Anaphat Sueakhamron 6480228
//Akkhrawin Nair 6580013
//Pakin Panawattanakul 6580043
//Pannawish Kriengyakul 6580117
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  

#define FIFO1 "./fifo1"
#define FIFO2 "./fifo2"
void signal_handler(int SIGNO)//when parent receive SIGTERM from child
{
    if (SIGNO == SIGTERM)
      exit(0);// we want the parent process to exit
}

int main(int argc,char **argv) 
{
  if (argc<2){
    printf("ERROR: not enough arguement\n");
    return -1; // if not enough arguemnt exit the program
  }
  int fd1,fd2; // file discriptor
  mkfifo(FIFO1,0666);//pipe1>2
  mkfifo(FIFO2,0666);//pipe2>1
  
  char buffer[BUFSIZ+1];
  memset(buffer,'\0',BUFSIZ); //set buffer to all NULL
  if (atoi(argv[1]) == 1 ) //program1
  {
    pid_t child = fork();
    
    switch(child) 
    {
      case -1 : 
        printf("Error, fail to create child process\n");
        break;
      
      case 0 : // child process1
        fd2 = open(FIFO2,O_RDONLY);//return file discriptor if pipe2>1
        while(strcmp(buffer,"end chat\n"))
        {
          memset(buffer,'\0',BUFSIZ);
          read(fd2,buffer,BUFSIZ);//read from pipe to buffer
          fputs(buffer,stdout);//puts message from buffer to stdout
        }
        // case : type end chat in ./chat 2
        // child read "end chat" and parent does not know so we need to send signal to parent
        unlink(FIFO1);//we know that parent process 2 will unlink FIFO2
        pid_t parent = getppid();//get parent pid to sent signal to terminate
        kill(parent,SIGTERM);// sent SIGTERM to parent
        raise(SIGTERM);//terminate itself
        break;  
      
      default : // parent process1 
        fd1 = open(FIFO1,O_WRONLY);//return file discriptor of pipe1>2
        signal(SIGTERM,signal_handler);// wait for SIGTERM from child in case send end chat from another parent
        do{
          memset(buffer,'\0',BUFSIZ);
          fgets(buffer,BUFSIZ,stdin);//get message from stdin to buffer
          write(fd1,buffer,BUFSIZ);//write from bufferto pipe
        }while(strcmp(buffer,"end chat\n"));
        // case : type end chat in ./chat 1
        //parent send "end chat"  and child does not know so we need to send signal to child
        unlink(FIFO1);//unlink pipe1
        kill(child,SIGTERM);//send SIGTERM to child
        raise(SIGTERM);//terminate itself
    }
  }

  if (atoi(argv[1]) == 2) //program2
  {
    pid_t child = fork();
    switch(child) 
    {
      
      case -1 : 
        printf("Error, fail to create child process\n");
        break;

      case 0 : // child process2
        fd1 = open(FIFO1,O_RDONLY); //return file discriptor of pipe1>2
        while(strcmp(buffer,"end chat\n"))
        {
          memset(buffer,'\0',BUFSIZ);
          read(fd1,buffer,BUFSIZ);//read from pipe to buffer
          fputs(buffer,stdout);//put message from buffer to stdout
        }
        // case : type end chat in ./chat 1
        // child read "end chat" and parent does not know so we need to send signal to parent
        unlink(FIFO2);//we know that parent process 1 will unlink FIFO1
        pid_t parent = getppid();//get parent pid to sent signal to terminate
        kill(parent,SIGTERM);// sent SIGTERM to parent
        raise(SIGTERM);//terminate itself
        break;  
      
      default : // parent process2
        fd2 = open(FIFO2,O_WRONLY); //return file discriptor if pipe2>1
        signal(SIGTERM,signal_handler);// wait for SIGTERM from child in case send end chat from another parent
        do{
            memset(buffer,'\0',BUFSIZ);
            fgets(buffer,BUFSIZ,stdin);//get message from stdin to buffer
            write(fd2,buffer,BUFSIZ);// wrtie from buffer to pipe
          }while(strcmp(buffer,"end chat\n"));
        // case : type end chat in ./chat 2
        //parent send "end chat"  and child does not know so we need to send signal to child
        unlink(FIFO2);//unlink pipe2
        kill(child,SIGTERM);//send SIGTERM to child
        raise(SIGTERM);//terminate itself
    }
  }
  return 0;
}