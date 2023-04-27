# OS PROJECT
Write a C program, which uses system calls and library functions in Unix, that takes multiple arguments representing paths to regular files / directories / symbolic links.


The program provides the following functionalities:


The program will pass through all the arguments.


For each file, depending on the file type, the name of the file and the type of the file will be displayed and after that an interactive menu will be displayed, with all the options available for that specific file type. 


The user should then, input the desired options for that file from the keyboard, as a single string. The options will be preceded by the ‘-’ sign and will contain multiple letters corresponding to each option. An example input from the keyboard looks like this: -nal. Note that, some options may require additional input information. 


After entering the options string, the information about that file will be printed to the standard output.


If one of the letters of the string is not a valid option an error message will be displayed and the menu will appear again. 


A) Regular file: name (-n), size (-d), hard link count (-h), time of last modification (-m), access rights (-a), create symbolic link (-l, this will wait for user input for the name of the link). The access rights will be displayed in the format:

        User: 

        Read - yes

        Write - yes

        Exec - no

        Group:

        Read - yes

        Write - no

        Exec - no

        Others:

        Read - yes

        Write - no

        Exec - no

B) Symbolic link: name (-n), delete symbolic link (-l), size of symbolic link (-d), size of target file (-t), access rights (-a). Note that if that the -l option is given, the other following options will no longer be performed


C) Directory: name (-n), size (-d), access rights (-a), total number of files with the .c extension (-c)



The parent process will create for each argument one child process that will handle the options introduced by the user (for each file type we have the corresponding options).


If the argument is a regular file with the .c extension, the parent will create one more process (a second child process) that will execute a script.


Script requirement:

Having a regular file with the .c extension given as argument, the script should compile the file and print at standard output the number of errors and the number of warnings.


If the argument is a directory, the parent will create a second child process which will execute a command for creating a file with the .txt extension and a specific name (e.g. dirName_file.txt)


The parent process must properly receive the return state of its child processes. All child processes must run in parallel with each other.
