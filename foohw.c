#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
  char block[512]; 
  int in, out, nread;
  in = open("file.in", O_RDONLY);
  out = open("file.out", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);

  int mid = lseek(in, 0, SEEK_END)/2;
  lseek(in,0,SEEK_SET);
  char null = '\0';
    
  while ((nread = read(in, block, sizeof(block))) > 0) 
    
    write(out, block, nread);
  lseek(out, mid-8, SEEK_SET);
  for(int i=0; i<16; i++)
    write(out, &null, 1);
  return 0;
}   