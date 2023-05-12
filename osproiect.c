#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/wait.h>
char path[1024];
char result[1024];
char buffer[100];
struct stat info;

#define bufferSize 200

struct errorAndWarning{
    int errors, warnings;
};

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

bool checkCF(char *name){
    char *extension = strrchr(name,'.');
    if(extension && !strcmp(extension,".c")){
            return true;
        }
    return false;
}
void checkEAndW(char *name, int createPipe[]){
    
    char buffer[1024];
    char command[1024];
    sprintf(command,"./script.sh %s", name);
    FILE* pipe= popen(command,"r");
    if(!pipe){
        perror("Couldn't open the pipe for script.sh");
        exit(0);
    }
    printf("\n");
    int ok = 1;
    int warnings = 0, errors = 0;
    while(fgets(buffer, 1024, pipe)){
        for(int i=0;i<strlen(buffer);i++){
            if(isdigit(buffer[i])){
                if(ok == 1){
                    warnings = warnings * 10 + buffer[i]-'0';
                }
                else{
                    errors = errors * 10 + buffer[i]-'0';
                }
            }
        }
        ok = 0;
    }
    struct errorAndWarning Data;
    Data.warnings = warnings;
    Data.errors = errors;
    close(createPipe[0]);
    write(createPipe[1],&Data,sizeof(Data));
    close(createPipe[1]);
    pclose(pipe);
}

void countLines(FILE *fis){
    int count;
    fscanf(fis,"%d",&count);
    
    printf("The file contains %d lines\n",count);
    pclose(fis);
}

int score2(struct errorAndWarning Data){
    int score;
    if(!Data.errors && !Data.warnings)
        score = 10;
        else if(Data.errors>0)
            score = 1;
            else if(!Data.errors && Data.warnings>10)
                score = 2;
                else if(!Data.errors && Data.warnings<=10)
                    score = 2+8*(10-Data.warnings)/10;
    return score;
}

void File(char* name, struct stat var)
{
    char options[10];
    int status;
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
    for(int i=0;i<strlen(options)-1; i++)
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
            exit(2);
        }else{
            if(i==strlen(options)-2){
                    
                for (int i = 0; i < strlen(options)-2; i++) {
                    pid_t child = waitpid(-1, &status, 0);

                    if (WIFEXITED(status)) {
                        int exit_code = WEXITSTATUS(status);
                        printf("\nThe process with PID %d has ended with the exit code %d\n", child, exit_code);
                    } else {
                        printf("\nThe process with PID %d has ended not normal\n", child);
                    }
                }
                int createPipe[2];
                if(pipe(createPipe) == -1){
                    perror("Couldn't create pipe\n");
                    exit(0);
                }
                pid_t pid2;
                pid2 = fork();

                if(pid2==0){
                    countLines(fis); 
                    exit(0);
                } else{
                        
                    if(checkCF(name)){
                        close(createPipe[1]);
                        struct errorAndWarning data;
                        int len = read(createPipe[0], &data, sizeof(data));
                        if(len!=sizeof(data)){
                            perror("Couldn't read from pipe\n");
                            exit(0);
                        }
                        else{
                            int score = score2(data);
                            FILE *f = fopen("grades.txt","a+");
                            if(!f){
                                perror("Couldn't open the grades file.\n");
                                exit(0);
                            }
                            else{
                                printf("Score was successfully written.\n");
                            }
                            fprintf(f,"File has the score: %d\n",score);
                        }
                        close(createPipe[0]);
                    }
                    wait(&status);
                    if (WIFEXITED(status)) {
                        int exit_code = WEXITSTATUS(status);
                        printf("\nThe process with PID %d has ended with the exit code %d\n", pid2, exit_code);
                    } else {
                        printf("\nThe process with PID %d has ended abnormally\n", pid2);
                    }
                        
                }
            }
                
        }
    }
    fclose(fis); 
} 

void Directory(char* name, struct stat var)
{
    DIR *dir=opendir(name);
    char options[10];
    struct dirent* entry;
    struct stat st;
    int status;
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
    for(int i=0;i<strlen(options); i++)
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
            exit(2);
        }else{
            if(i==strlen(options)-2){
                for (int i = 0; i < strlen(options)-1; i++) {
                    pid_t child = waitpid(-1, &status, 0);

                    if (WIFEXITED(status)) {
                        int exit_code = WEXITSTATUS(status);
                        printf("\nThe process with PID %d has ended with the exit code %d\n", child, exit_code);
                    } else {
                        printf("\nThe process with PID %d has ended not normal\n", child);
                    }
                }
            }
        }
    }
    closedir(dir);
}

void SymbolicLink(char* name, struct stat var)
{
    int status;
    char options[10];
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
    for(int i=0;i<strlen(options); i++)
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
            exit(2);
        } else{
            if(i==strlen(options)-2){
                for (int i = 0; i < strlen(options)-1; i++) {
                    pid_t child = waitpid(-1, &status, 0);

                    if (WIFEXITED(status)) {
                        int exit_code = WEXITSTATUS(status);
                        printf("\nThe process with PID %d has ended with the exit code %d\n", child, exit_code);
                    } else {
                        printf("\nThe process with PID %d has ended not normal\n", child);
                    }
                }
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