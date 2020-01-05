#include "threadbank.h"
#include "desk.h"

 // Signal handler
void sig_handler(int signo) {
    if (signo == SIGINT) { // CTRL+C
        printf("\nControl+C pressed, exiting the program.\n");
        //exec("rm *.bank"); // run rm *.bank
        if (munmap(queue_arr, sizeof(int)*n) == -1) { // Remove mapped array
            perror("munmap failed with error: ");
        }
        char cmd[10] = {"rm *.bank"}; // Define command to remove all bank notes
        system(cmd); // Remove all bank notes
        kill(0,SIGTERM); // Terminate main program
        exit(EXIT_FAILURE);
    }
    else if (signo == SIGINFO) { // CTRL+T, master desk signal
        *flag = 1; // Raise the global flag
        printf("CTRL+T pressed, waiting for all desks to report to the master.\n");
        int deposit_master, withdraw_master = 0;
        for (int i = 0; i < n; i++) {
            char read_buffer[SIZE];
            while (read(fd2[2*i+READ], read_buffer, SIZE) > 0) {
                printf("%s\n", read_buffer);
            }
            fprintf(stdout, "Got reading from desk %d\n", i);
        }
    }
}

int main(int argc, char *argv[]) {
    struct sigaction act; // Initialize signal-handler
    memset(&act, 0, sizeof(act)); // Set signal handler value as zero
    act.sa_handler = sig_handler;

    sigaction(SIGINT,  &act, 0); // Initialize SIGINT handler

    if (!argv[1]) {
        printf("Desk number required as variable. Exiting.\n");
        exit(EXIT_FAILURE);
    }
    n = atoi(argv[1]);
    if (n <= 0) { printf("Invalid input. Exiting.\n"); exit(1); }
    printf("Welcome to ThreadBank manager!\n");
    pid_t pid_c = 0; // PID child

    queue_arr = (int *)mmap(NULL, sizeof(int)*n, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (queue_arr == MAP_FAILED) {
        perror("Error mmapping the queue: ");
        exit(EXIT_FAILURE);
    }

    flag = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    //*flag = 0;
    if (flag == MAP_FAILED) {
        perror("Error mmapping the flag: ");
        exit(EXIT_FAILURE);
    }
    //if (!queue_arr) { printf("Allocating memory failed. Exiting.\n"); exit(1); }
    fd1 = malloc(2*n*sizeof(fd1)); // Master to desk, d1[0] to read and fd1[1] to write
    fd2 = malloc(2*n*sizeof(fd2)); // Desk to master
    printf("check1\n");

    for (int i = 0; i < n; i++) { // Create all processes and open pipes
        ((int*)queue_arr)[i] = 0; // Is it needed as calloc initializes as zero?
        if (pipe(fd1 + 2*i) == -1) { fprintf(stderr, "Pipe 1 Failed\n"); } // Open pipe fd1
        if (pipe(fd2 + 2*i) == -1) { fprintf(stderr, "Pipe 2 Failed\n"); } // Open pipe fd2
        pid_c = fork(); // Fork process
        if (pid_c < 0) { printf("Fork failed.\n"); }
        else if (pid_c == 0) { // Child process
            close(fd1[2*i+WRITE]); // Close writing end of fd1
            close(fd2[2*i+READ]); // Close reading end of fd2
            desk(i, fd1, fd2); // Child becomes the counter - passing the number of desk and pipes
        }
        else { // Parent process
            sigaction(SIGINFO,  &act, 0); // Initialized SIGINFO handler on the master thread for requesting deposits/withdrawals
            close(fd1[2*i+READ]); // Close reading end of fd1
            close(fd2[2*i+WRITE]); // Close writing end of fd2
        }
    }
    (n == 1) ? printf("1 desk is serving.\n") : printf("%d desks are serving.\n", n);
    printf("The bank offers four different services:\nbalance (l), withdraw (w), transfer (t), deposit (d).\n");

    char *request = malloc(INPUT_SIZE); // Initialize request
    if (request == NULL) { printf("No memory.\n"); }

    int s;

    while(signal(SIGINT, sig_handler) != SIG_ERR || signal(SIGINFO, sig_handler) != SIG_ERR) {
        if (fgets(request, INPUT_SIZE, stdin) != NULL && strlen(request) > 1) {
            request[strlen(request) - 1] = '\0'; // Convert last characther to NUL byte
            s = shortestline(); // Get the desk with shortest line
            queue_arr[s]++;
            write(fd1[2*s+WRITE], request, strlen(request)+1); // Write request to pipe, include NUL byte
        }
    }
}
