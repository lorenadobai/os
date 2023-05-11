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

#define bufferSize 200

void RFTimeModification(char *path)
{
    struct stat attr;
    stat(path, &attr);
    printf("Last modified time: %d", ctime(&attr.st_mtime));
}


void RFSymbolic(char *path)
{
     const char* linkPath;
     printf("the name of the link ");
     scanf("%s",linkPath);
     int result = symlink(path, linkPath);
     if (result == 0) {
        printf("Symbolic link created successfully.\n");
     } else {
        perror("Failed to create symbolic link");
     }
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

void Access(struct stat var)
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
    for(int i=0;i<6; i++)
    {
        if(pid==0){
            switch (j)
            {
                case 1 : printf("File name is %s\n",name); break;
                case 2 : printf("The file is %ld bytes long\n", (long)var.st_size); break;
                case 3 : printf("The file has %ld hard links\n", (long)var.st_nlink); break;
                case 4 : RFTimeModification(path); break;
                case 5 : Access(var); break;
                case 6 : RFSymbolic(fis); break;
                default: printf("it is not a choice");
                break;
            }
        }
    }
}

void Directory(char* name, struct stat var)
{
    DIR *dir=opendir(name);
    struct dirent* entry;
    struct stat st;
    char file[bufferSize];
    int totalFiles = 0;
    if(dir==NULL)
    {
        perror("dir cannot be opened!\n");  
        exit(2);
    }
    while ((entry = readdir(dir)) != NULL) {
        sprintf(file, "%s/%s", name, entry->d_name);
        if (lstat(file, &st) == -1) {
            perror("lstat");
            continue;
        }
        if (S_ISREG(st.st_mode)) {
            if (strstr(entry->d_name, ".c") != NULL) {
                totalFiles++;
            }
        }
    }
    char c[2];
    printf("Choose one for directories:name (-n), \nsize (-d),\n access rights (-a),\n total number of files with the .c extension (-c)");
    scanf("%s",c);
    int j=makeInt(c);
    pid_t pid;
    pid = fork();
    if(pid<0){
        fprintf(stderr, "Failed to fork\n.");
        exit(2);
    }
    for(int i=0;i<4; i++)
    {
        if(pid==0){
            switch (j)
            {
                case 1 : printf("Dir name is %s\n",name); break;
                case 2 : printf("The dir is %ld bytes long\n", (long)var.st_size); break;
                case 3 : Access(var); break;
                case 4 : printf("Total .c files: %d\n", totalFiles); break;
                default: printf("it is not a choice");
                break;
            }
        }
    }
    closedir(dir);
}

void SymbolicLink(char* name, struct stat var)
{
    if (lstat(name, &var) == -1)
    {
        perror("lstat");
        exit(2);
    }
    char c[2];
    printf("Choose one for Symbolic link: name (-n), delete symbolic link (-l), size of symbolic link (-d), size of target file (-t), access rights (-a)");
    scanf("%s",c);
    int j=makeInt(c);
    pid_t pid;
    pid = fork();
    if(pid<0){
        fprintf(stderr, "Failed to fork\n.");
        exit(2);
    }
    for(int i=0;i<5; i++)
    {
        if(pid==0){
            switch (j)
            {
                case 1 : printf("Name: %s\n", name);break;
                case 2 : if (unlink(name) == -1) 
                        {
                            perror("unlink");
                        }
                        i=5;
                        break;
                case 3 : printf("Size of symbolic link: %ld bytes\n", var.st_size); break;
                case 4 : printf("Size of target file: %ld bytes\n", stat(name, &var) == -1 ? -1 : var.st_size); break;
                case 5 : Access(var); break;
                default: printf("it is not a choice");
                break;
            }
        } 
    }
}

int main(int argc,char *argv[])
{
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
            Directory(argv[i], var);
        }
        if(S_ISLINK(var.st_mode))
        {
            SymbolicLink(argv[i],var);
        }
    }
}