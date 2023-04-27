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

void RFTimeModification(FILE *f)
{

}

void RFAccess(FILE *f)
{

}

void RFSymbolic(FILE *f)
{

}

int main(int argc,char *argv[])
{
    FILE *fis;
    DIR *dir;
    struct stat info;
    if(argc<=2)
    {
        perror("not enough arguments");
        exit(2);
    }
    if(fis=open(argv[2], O_RDONLY)<0){
        perror("fisOut cannot be opened!\n");
        exit(0);
    }
    else
    {
        char c;
        printf("choose one for files: name (-n), size (-d), hard link count (-h), time of last modification (-m), access rights (-a), create symbolic link (-l, this will wait for user input for the name of the link).");
        scanf("%c",c);
        switch (c)
        {
        case '-n': RegularFileName(fis); break;
        case '-d': RegularFileSize(fis); break;
        case '-h': RFHardLinkCount(fis); break;
        case '-m': RFTimeModification(fis); break;
        case '-a': RFAccess(fis); break;
        case '-l': RFSymbolic(fis); break;
        default:
            break;
        }
    }
}