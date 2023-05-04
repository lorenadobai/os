#include <stdlib.h>
#include <ctype.h> 
#include <errno.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <wait.h>
char path[1024];
char result[1024];
char buffer[100];
struct stat info;

//verify fis: stat(<0), !S_ISREG
//verify pipes: 

void RegularFileName(FILE *f)
{
    int fd = fileno(f); 
    sprintf(path, "/proc/self/fd/%d", fd);
    memset(result, 0, sizeof(result));
    readlink(path, result, sizeof(result)-1);
    printf("%s\n", result);
}

void RegularFileSize(FILE *f)
{
    struct stat st;
    stat(f, &st);
    int size = st.st_size;
}

void RFHardLinkCount(FILE *f)
{
    struct stat st;
    if (stat(f, &st) == -1) 
    {
        perror("stat doesn't funtion for hard links");
        exit(0);
    }
    else 
        printf("%s has %d hard links\n", f, st.st_nlink);
}

void RFTimeModification(char *path)
{
    struct stat attr;
    stat(path, &attr);
    printf("Last modified time: %s", ctime(&attr.st_mtime));
}

void RFAccess(FILE *f)
{

}

void RFSymbolic(FILE *f)
{

}

int makeInt(char *c)
{
    int j=0;
    if(strcmp(c,"-n")==0)
        j=1;
    else if(strcmp(c,"-d")==0)
            j=2;
    else if(strcmp(c,"-h")==0)
            j=3;
    else if(strcmp(c,"-m")==0)
            j=4;
    else if(strcmp(c,"-a")==0)
            j=5;
    else if(strcmp(c,"-l")==0)
            j=6;
    return j;
}

int main(int argc,char *argv[])
{
    FILE *fis;
    DIR *dir;
    int verific=0;
    if(argc<2)
    {
        perror("not enough arguments");
        exit(2);
    }
    for(int i=0;i<argc;i++)
    {
        if(lstat(path,buffer)){
            perror("fis cannot be opened!\n");
            exit(0);
        }
        else
        {
          char c[2];
          printf("Choose one for files:\n name (-n),\n size (-d),\n hard link count (-h),\n time of last modification (-m),\n access rights (-a),\n create symbolic link (-l, this will wait for user input for the name of the link).\n");
          scanf("%s",c);
          int j=makeInt(c);
          switch (j)
          {
              case 1 : RegularFileName(fis); break;
              case 2 : RegularFileSize(fis); break;
              case 3 : RFHardLinkCount(fis); break;
              case 4 : RFTimeModification(fis); break;
              case 5 : RFAccess(fis); break;
              case 6 : RFSymbolic(fis); break;
              default: printf("it is not a choice");
              break;
           }
        }
    }
}