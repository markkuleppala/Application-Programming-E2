#include "threadbank.h"
#include "desk.h"

void sig_handler(int signo) { // Signal handlers
    if (signo == SIGINT){ // CTRL+C to abort the program
        if (pid_p == getpid()) { // Parent process
            printf("\nControl+C pressed, exiting the program.\n");
            free(fd1); free(fd2); free(request); // Free the allocated variables
            if (munmap(queue_arr, sizeof(int)*n) == -1) { // Remove mapped array
                perror("munmap of queuer_arr failed with error: ");
            }
            if (munmap(flag, sizeof(int)*n) == -1) { // Remove mapped array
                perror("munmap flag failed with error: ");
            }
            char cmd1[10] = {"rm *.bank"}; // Define command to remove all bank notes
            char cmd2[24] = {"rm transaction_log.txt"}; // Define command to remove transaction log
            system(cmd1); // Remove all bank notes
            system(cmd2); // Remove transaction log
            kill(0,SIGTERM); // Terminate main program
            exit(EXIT_SUCCESS);
        }
        else {
            fprintf(stdout, "\nDesk # %d closing.\n", i+1); // Notify use about closing the desk
            exit(EXIT_SUCCESS);
        }
    }
    else if (signo == SIGINFO && pid_p == getpid()) { // CTRL+T, master desk signal
        deposit_master = 0; // Reset deposit master value
        withdraw_master = 0; // Reset withdraw master value
        *flag = 1; // Raise the global flag
        printf("CTRL+T pressed, waiting for all desks to report to the master.\n");
        for (int i = 0; i < n; i++) { // Loop through all desk and wait for the d/w report
            int arr[SIZE]; // Initialize read buffer array
            while (read(fd2[2*i+READ], arr, sizeof(arr)) <= 0) { // Wait for the report
            }
            fprintf(stdout, "d: %d w: %d from # %d\n", arr[0], arr[1], i+1); // Print the report from the desk
            deposit_master += arr[0]; // Increment the deposit master value
            withdraw_master += arr[1]; // Increment the withdraw master value
        }
        *flag = 0; // Set global flag to zero
        printf("All desks reported back. Deposits %d, withdraws %d.\n", deposit_master, withdraw_master);
    }
}

void master_desk(void) {
    int s; // Shortest line help variable

    while(1) { // Wait for the user input
        if (fgets(request, INPUT_SIZE, stdin) != NULL && strlen(request) > 1) { // User input given
            request[strlen(request) - 1] = '\0'; // Convert last characther to NUL byte
            s = shortestline(); // Get the desk with shortest line
            queue_arr[s]++; // Increase the queue of desk s by one
            write(fd1[2*s+WRITE], request, strlen(request)+1); // Write request to pipe, include NUL byte
        }
    }
}

int main(int argc, char *argv[]) {
    struct sigaction act; // Initialize signal-handler
    memset(&act, 0, sizeof(act)); // Set signal handler value as zero
    act.sa_handler = sig_handler; // Signal handler help variable

    sigaction(SIGINT,  &act, NULL); // Initialize SIGINT handler

    if (!argv[1]) { // No desk number variable given on startup
        printf("Desk number required as variable. Exiting.\n");
        exit(EXIT_FAILURE);
    }
    n = atoi(argv[1]); // Read number of desk from the argument
    if (n <= 0) { printf("Invalid input. Exiting.\n"); exit(1); } // Value not a positive integer (1-N)
    printf("Welcome to ThreadBank manager!\n");
    pid_t pid_c = 0; // PID child

    char init[SIZE]; // Initilize helper char array
    sprintf(init, "Opened the Threadbank with %d desk(s)\n", n); // Initilize text string to log creation of new account
    pid_logger = fork(); // Fork the process
    if (pid_logger < 0) { perror("Fork failed, skip logging."); } // Failed fork
    else if (pid_logger == 0) { execl("./logger", init, (char*) NULL); } // Child process, write account creation to log

    queue_arr = (int *)mmap(NULL, sizeof(int)*n, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); // Initialize queue array for tracking queue per desk
    if (queue_arr == MAP_FAILED) { // Memory mapping failed
        perror("Error mmapping the queue: ");
        exit(EXIT_FAILURE);
    }

    flag = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); // Initialize global flag for master desk halt
    if (flag == MAP_FAILED) { // Memory mapping failed
        perror("Error mmapping the flag: ");
        exit(EXIT_FAILURE);
    }

    *flag = 0; // Set global flag to zero

    fd1 = malloc(2*n*sizeof(fd1)); // Master to desk, d1[0] to read and fd1[1] to write
    fd2 = malloc(2*n*sizeof(fd2)); // Desk to master

    for (i = 0; i < n; i++) { // Create all processes and open pipes
        ((int*)queue_arr)[i] = 0; // Is it needed as calloc initializes as zero?
        if (pipe(fd1 + 2*i) == -1) { fprintf(stderr, "Pipe 1 Failed\n"); } // Open pipe fd1
        if (pipe(fd2 + 2*i) == -1) { fprintf(stderr, "Pipe 2 Failed\n"); } // Open pipe fd2
        if (fcntl(*fd1, F_SETFL, O_NONBLOCK) == -1) { fprintf(stderr, "Setting fd1 to non-blocking failed\n"); } // Set fd1 to non-blocking
        if (fcntl(*fd2, F_SETFL, O_NONBLOCK) == -1) { fprintf(stderr, "Setting fd2 to non-blocking failed\n"); } // Set fd2 to non-blocking
        pid_p = getpid(); // Get parent PID
        pid_c = fork(); // Fork process
        if (pid_c < 0) { printf("Fork failed.\n"); }
        else if (pid_c == 0) { // Child process
            desk(i, fd1, fd2, flag); // Child becomes the counter - passing the number of desk and pipes
        }
        else { // Parent process
            sigaction(SIGINFO,  &act, NULL); // Initialized SIGINFO handler on the master thread for requesting deposits/withdrawals
            close(fd1[2*i+READ]); // Close reading end of fd1
            close(fd2[2*i+WRITE]); // Close writing end of fd2
        }
    }
    (n == 1) ? printf("1 desk is serving.\n") : printf("%d desks are serving.\n", n);
    printf("The bank offers four different services:\nbalance (l), withdraw (w), transfer (t), deposit (d).\n");

    request = malloc(INPUT_SIZE); // Initialize request
    if (request == NULL) { printf("No memory.\n"); exit(EXIT_FAILURE); } // Malloc failed, out of memory

   master_desk();
}
