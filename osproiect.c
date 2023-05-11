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

void RFTimeModification(char *path)
{
    struct stat attr;
    stat(path, &attr);
    printf("Last modified time: %s", ctime(&attr.st_mtime));
}

void RFAccess(FILE *f, struct stat var)
{
    printf("User:\n    Read- %s\n    Write- %s\n    Exec- %s\n", 
        (var.st_mode & S_IRUSR) ? "yes" : "no",
        (var.st_mode & S_IWUSR) ? "yes" : "no",
        (var.st_mode & S_IXUSR) ? "yes" : "no");
    printf("Group: \n    Read- %s\n    Write- %s\n    Exec- %s\n", 
        (var.st_mode & S_IRGRP) ? "yes" : "no",
        (var.st_mode & S_IWGRP) ? "yes" : "no",
        (var.st_mode & S_IXGRP) ? "yes" : "no");
    printf("Others: \n    Read- %s\n    Write- %s\n    Exec- %s\n", 
        (var.st_mode & S_IROTH) ? "yes" : "no",
        (var.st_mode & S_IWOTH) ? "yes" : "no",
        (var.st_mode & S_IXOTH) ? "yes" : "no");
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

void File(char* name, struct stat var)
{
    FILE *fis=fopen(name, "r");
    if(fis==NULL){
        perror("fis cannot be opened!\n");  
        exit(2);
    }
    char c[2];
    printf("Choose one for files:\n name (-n),\n size (-d),\n hard link count (-h),\n time of last modification (-m),\n access rights (-a),\n create symbolic link (-l, this will wait for user input for the name of the link).\n");
    scanf("%s",c);
    int j=makeInt(c);
    pid_t pid;
    pid = fork();
    if(pid<0){
    fprintf(stderr, "Failed to fork\n.");
            exit(2);
    }
    if(pid==0){
        switch (j)
        {
            case 1 : printf("File name is %s\n",name); break;
            case 2 : printf("The file is %ld bytes long\n", (long)var.st_size); break;
            case 3 : printf("The file has %ld hard links\n", (long)var.st_nlink); break;
            case 4 : RFTimeModification(path); break;
            case 5 : RFAccess(fis,var); break;
            case 6 : RFSymbolic(fis); break;
            default: printf("it is not a choice");
            break;
        }
    }
}

int main(int argc,char *argv[])
{
    DIR *dir;
    if(argc<2)
    {
        perror("not enough arguments");
        exit(2);
    }
    for(int i=1;i<argc;i++){

        struct stat var;
        if(stat(argv[i], &var) == -1){
            perror("stat does not work");
            exit(2);
        }
        if(S_ISREG(var.st_mode)){
                File(argv[i], var);
        }
        if(S_ISDIR(var.st_mode)){
                Dir(argv[i], var);
        }
    }
}