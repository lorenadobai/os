#include <stdlib.h>
#include <ctype.h> 
#include <errno.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

int main(int argc,char *argv[])
{
    FILE *fisOut;
  struct stat info;
   if(argc!=3)
   {
    perror("not enough arguments");
    exit(2);
   }
   if(fisOut=open(argv[2], O_RDONLY)<0){
        perror("fisOut cannot be opened!\n");
        exit(0);
   }
   if(fstat(fisOut,&info)<0){
        perror("fstat dind't work\n");
        exit(0);
   }
    if(!(S_ISREG(info.st_mode))){
        perror("S_ISREG dind't work\n");
        exit(0);
   }
   if( stat( fisOut, &info) != -1) 
{
    if( S_ISREG( info.st_mode ) != 0 )
    {
        printf( "%s is a file", fisOut ) ;
    }
    else
    {
        printf( "%s is not a file", fisOut ) ;
    }
}
}