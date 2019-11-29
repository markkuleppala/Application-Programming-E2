/* Make a library that can capture normal output (for example, printf()) of other
programs to a separate output process or thread. This output process should
continue to post-process the output of programs by, for example, writing it out to
a log file.

The implementation should be as simple as possible to add to existing programs, and it must also
support child processes spawned by the programs. Name of the log file should be
able to be given in the old program.

Write also a threaded program to test the library. */

/* Communication between processes or programs
• Thread creation
• Use of mutex
• Thread synchronization */

#include <stdio.h> // popen, scanf
#include <time.h> // time_str
#include <sys/types.h> // getpid
#include <stdlib.h> // wait
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h> // getpid, access, pipe
#include <string.h> // strlen

int logmsg(char *name, int pid, char *message, char *__progname);
int readpipe();
void forkandlisten(void);

int main(void) {
    //extern char *__progname;
    //printf("%s\n", __progname);
    //pid_t pid = getpid();
    //char *fname = "printf.log";
    //char *message  = "Logging something beta delta.";
    //int ret = logmsg(fname, pid, message, __progname);
    forkandlisten();
    //prinft("%d", ret2);
    //return ret;
}

int logmsg(char *fname, int pid, char *message, char *__progname) {
    FILE *f;
    time_t mytime = time(NULL); // Initialize mytime variable
    if (access(fname, F_OK ) == -1 ) { // Log file doesn't exist
        f = fopen(fname, "a"); // Open log file with append rights
        if (!f) { // Opening log file failed
            printf("Writing log to %s failed.\n", fname);
            return -1; 
        }
        else { // File created succesfully
            fprintf(f, "TIMESTAMP\t\t  PID    PROGNAME       MESSAGE\t\n"); // Write header row
            char *time_str = ctime(&mytime); // Get current time for timestamp
            time_str[strlen(time_str)-1] = '\0'; // Delete newline from the end
            fprintf(f, "%s  %d  %s           %s\n", time_str, pid, __progname, message); // Write message
        }
    }
    else { // Log file exists
        f = fopen(fname, "a");
        char *time_str = ctime(&mytime); // Get current time for timestamp
        time_str[strlen(time_str)-1] = '\0'; // Delete newline from the end
        fprintf(f, "%s  %d  %s           %s\n", time_str, pid, __progname, message); // Write message
    }
    fclose(f); // Closing opened file
    return 1; // Writing log succesfully
}

/*
int readpipe() {
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        printf("Opening pipe failed.\n");
    }
    FILE *file = popen("write 2>&1", "r"); // Open pipe to main, forwarding stderr to stdout
    char buffer[100];
    read(file, buffer, 100);
    //read(pipefd[0], buffer, 100);
    //fgets(buffer, 100, file); // or fscanf
    printf("%s", buffer);
    return 1;
} */

void forkandlisten() {
    extern char *__progname;
    pid_t pid_c = 0;
    int fd[2];
    char buffer[4096]; // OVERFLOW WARNING!
    char fname[100];
    
    //size_t buffer_size = 4096;
    ssize_t bytes_read = 0;
    if (pipe(fd) < 0) { printf("Creating pipe failed.\n"); } // Pipe created
    else {
        pid_c = fork(); // Process forked, returns 0 to child process and child's PID to parent process
        if (pid_c < 0) { printf("Fork failed.\n"); }
        else if (pid_c == 0) { // This is child process
            close(fd[0]); // Closing read-descriptor
            close(STDOUT_FILENO); // Closing stdout
            dup(fd[1]); // Forwarding stdout to pipe
            printf("Hi Daddy!");
            wait(NULL);
        }
        else { // This is parent process
            printf("Filename to write log in: ");
            scanf("%s", fname);
            close(fd[1]); // Closing write-descriptor5
            while ((bytes_read = read(fd[0], buffer, 4096)) > 0) { // Read bytes from pipe
                puts(buffer); // Output read line to buffer
                //fflush(stdout);
            }
            printf("Child sent message: %s\n", buffer);
            //char *fname = "printf.log";
            logmsg(fname, pid_c, buffer, __progname);
            fflush(stdout);
            //int status;
            //waitpid(-1,&status,0);



            //close(pipefd[1]); // Closing write-descriptor when exiting
        }
    }
}