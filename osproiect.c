#include <stdlib.h>
#include <ctype.h> 
#include <errno.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc,char *argv[])
{
     if(argc!=3)
   {
    perror("not enough arguments");
    exit(2);
    
   }
}