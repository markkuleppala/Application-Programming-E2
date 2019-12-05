#include <stdio.h> /* scanf, printf, fgets */
#include <string.h> /* strtok */
#include <stdlib.h>  /* atof */
#include <unistd.h> 
#include <pthread.h> /* pthread_create */
#include <sys/types.h> /* gettid */
#include <math.h> /* log10, abs, floor */
#include <fcntl.h> /* fcntl */
#include <signal.h> /* signal */
#include "threadbank.h"

#define SIZE 1024
#define INPUT_SIZE 1024
#define READ 0
#define WRITE 1

int *queue_arr; // vaikko char

void sig_handler(int signo) { // Catching signals
    if (signo == SIGINT) { 
        printf("Control-C pressed, exiting the program.\n");
        free(queue_arr);
        kill(0,SIGTERM);
        exit(EXIT_FAILURE);
    }
} 

void *counter(void *vargp) {
    //int *id = (int *)vargp;
    printf("This is counter number %d with PID %d.\n", (int)pthread_self(), (int)getpid());
    return NULL;
}
/*
void forkcounter(int i, int fd[]) {
    pid_t pid_c = 0;
    //pthread_t thread_id;
    //char write_buffer[SIZE];
    pid_c = fork();
    if (pid_c < 0) { printf("Fork failed.\n"); }
    else if (pid_c == 0) { // Children
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, counter, (void*)&thread_id);
        //pthread_join(thread_id, NULL);
        close(fd[2*(i-1)]); 
        // Open pipes based on i
        //write(fd[2*(i-1) + 1], write_buffer, SIZE);
        printf("Counter number %d created with PID %d.\n", i, (int)getpid());
        exit(1);
    }
    else { // Parent
        // Log creation of counter n
        pipe(&fd[2*(i-1)]);
        close(fd[2*(i-1) + 1]);
    }
}
*/

double getlastline(char *account) { // Get balance - Check that file exists, if not, create an empty one
    //char line[1024]={0,};
    char tmp[1024];
    double balance = 0;
    char *account_name = malloc(sizeof(char)*strlen(account)+6); // Allocating array
    sprintf(account_name, "%s.bank", account); // Writing id + .bank
    printf("account number: %d\n", atoi(account));
    FILE *f;
    if (access(account_name, F_OK) == -1) { // File doesn't exist
        printf("Creating a new account.\n");
        f = fopen(account_name, "a");
        fprintf(f, "%.2f", balance);
    }
    else {
        f = fopen(account_name, "r");
        while (!feof(f)) {
            fgets(tmp, 1024, f);
        }
        balance = atof(tmp);
        //while (fgets(line, 1024, f)) {} // Search the last line
    }
    fclose(f);
    free(account_name);
    return balance;
}

void write_balance(char *account, double *value) {
    char *account_name = malloc(sizeof(char)*strlen(account)+6); // Allocating array
    sprintf(account_name, "%s.bank", account); // Writing id + .bank
    FILE *f = fopen(account_name, "ab+");
    fprintf(f, "\n%.2f", *value);
    printf("writing to %s value %.2f\n", account_name, *value);
    fclose(f);
    free(account_name);
}

double balance(char *number) {
    double balance = getlastline(number);
    printf("Balance of %s is: %.2f.\n", number, balance);
    return balance;
}

double deposit(char *account, char *value) { // int *number, int *amount) { 

    // Write lock
    double new_balance = getlastline(account) + atof(value);
    write_balance(account, &new_balance);
    // Write lock away
    return new_balance;
}

double withdraw(char *account, char *value) {

    // Write lock, use exec

    double balance = getlastline(account);
    if (balance >= atof(value)) {
        printf("Withdraw balance in %s is %f\n", account, balance);
        double new_balance = balance - atoi(value);
        // Write new balance to file
        write_balance(account, &new_balance);
    }
    else {
        printf("Insufficient value on the account.\n");
        return -1;
    }
    // Write lock away, use exec
    return 1;
}

double transfer(char *account1, char *account2, char *value) {
    // Write locks (and read locks), use exec
    double balance1 = getlastline(account1);
    if (balance1 >= atoi(value)) {
        withdraw(account1, value);
        deposit(account2, value);
    }
    else {
        printf("Insufficient value on the account.\n");
        return -1;
    }
    // Write locks away, use exec
    return 1;
}

int shortestline(void) {
    int i;
    int location = 0;
    int min = queue_arr[0];
    size_t s = sizeof(*queue_arr)/sizeof(queue_arr[0]);
    for (i = 1; i < s; i++) {
        if (queue_arr[i] < min) {
            min = queue_arr[i];
            location = i;
        }
    }
    return location;
}

struct Data {
    char *readbuffer;
    int d;
};

void *handlerequest(void *data) {
    printf("voidData->readbuffer %s\n", ((struct Data *)data)->readbuffer);
    printf("voidData->d %d\n", ((struct Data *)data)->d); 
    char *ptr = strtok(((struct Data *)data)->readbuffer, " ");
    int i_max;
    if (strcmp(ptr, "l") == 0) { i_max = 2; }
    else if (strcmp(ptr, "t") == 0) { i_max = 4; }
    else { i_max = 3; }
    char *action[i_max+1]; // malloc?
    for (int i = 0 ; i < i_max; i++) { // Read the request
        action[i] = ptr;
        ptr = strtok(NULL, " ");
    }
    if (strcmp(action[0], "l") == 0) { // l - give balance
        double l = balance(action[1]);
    }
    else if (strcmp(action[0], "d") == 0) { // d - deposit
        //double d = deposit(action[1], action[2]);
        if (deposit(action[1], action[2])) {
            //deposit_var = atoi(action[2]);
            ((struct Data *)data)->d = atoi(action[2]);
            //printf("deposit_var %d\n", deposit_var); 
            //pthread_exit((void *)&deposit_var);
        }
    }
    else if (strcmp(action[0], "w") == 0) { // w - withdraw
        double w = withdraw(action[1], action[2]);
    }
    else if (strcmp(action[0], "t") == 0) { // t - transfer
        double t = transfer(action[1], action[2], action[3]);
    }
    else { // Unknown request
        printf("Invalid request!\n");
    }
    //pthread_exit((void *)&deposit_var);
    return NULL;
}

void desk(int i, int fd1[], int fd2[]) {
    pthread_t thread_id;
    //pid_t pid_c = -1;
    //int status;
    char read_buffer[SIZE];
    double deposit_counter = 0;
    double withdraw_counter = 0;
    //void *d;
    //d = (int *)malloc(sizeof(int));
    int d = 0;
    struct Data data;
    //pthread_create(&thread_id, NULL, counter, (void*)&thread_id);
    //close(fd[2*i+WRITE]);
    //printf("Counter number %d created with TID %d.\n", i, (int)thread_id); // Does not return the correct thread value
    while(1) { // Get task from master thread and handle the queue
        if (read(fd1[2*i], read_buffer, SIZE) > 0) { // Read task to queue from even pipe - Add here the global master thread flag checker before continuing
            //printf("readbuffer %s of %d\n", read_buffer, i);
            if ((strlen(read_buffer) > 0) && (read_buffer[strlen(read_buffer) - 1] == '\n')) { // What's this for?
                read_buffer[strlen(read_buffer) - 1] = '\0';
                printf("Read buffer: %s of %d\n", read_buffer, i);
            }
            //pid_c = fork(); // Handle the request
            //if (pid_c < 0) { printf("Fork failed.\n"); }
            //else if (pid_c == 0) { // Children
                //printf("Children number: %d\n", i);
            d = 0;
            data.readbuffer = read_buffer;
            data.d = d;
            pthread_create(&thread_id, NULL, handlerequest, (void*)&data); // Removed &
            pthread_join(thread_id, NULL);//(void**)&d);
            printf("deposit %d\n", data.d);
        }
    }
}

int main(void) {
    int n; char term;
    //char read_buffer[SIZE];
    //pthread_t thread_id;
    printf("Welcome to ThreadBank manager!\nHow many desk do we have open today? (1-N): "); // Ask for open desks
    if (scanf("%d%c", &n, &term) != 2 || term != '\n') { printf("Invalid input!\n"); exit(1); } // Invalid input
    else if (n < 1) { printf("Apparently we are closed today. Until tomorrow!\n"); exit(1); } // 0
    else { // 1..N
        pid_t pid_c = 0; // PID child
        queue_arr = calloc(n, sizeof *queue_arr);
        if (!queue_arr) { printf("Allocating memory failed. Exiting.\n"); exit(1); }
        int fd1[2*n]; // Master to desk, d1[0] to read and fd1[1] to write
        int fd2[2*n]; // Desk to master
        int i;
        //printf("n %d\n", n);
        for (i = 0; i < n; i++) { // Create all processes and open pipes
            if (pipe(fd1) == -1) { fprintf(stderr, "Pipe 1 Failed\n"); } // Open pipe fd1
            if (pipe(fd2) == -1) { fprintf(stderr, "Pipe 2 Failed\n"); } // Open pipe fd2
            pid_c = fork(); // Fork process
            if (pid_c < 0) { printf("Fork failed.\n"); }
            else if (pid_c == 0) { // Child process
                close(fd1[WRITE]); // Close writing end of fd1
                close(fd2[READ]); // Close reading end of fd2
                desk(i, fd1, fd2); // Child becomes the counter - replace with exec?
            }
            else { // Parent process
                close(fd1[2*i+READ]); // Close reading end of fd1
                close(fd2[2*i+WRITE]); // Close writing end of fd2
            }
        }
        printf("Great! The bank is open!\n");
        printf("The bank offers four different services:\nbalance (l), withdraw (w), transfer (t), deposit (d).\n");

        char *request = malloc(INPUT_SIZE); // Initialize request
        if (request == NULL) { printf("No memory.\n"); }
        //queue_arr[0] = 2;
        //printf("%d\n", shortestline());


        while(signal(SIGINT, sig_handler) != SIG_ERR) {
            if (fgets(request, INPUT_SIZE, stdin) != NULL) {
                if ((strlen(request) > 0) && (request[strlen(request) - 1] == '\n')) {
                    request[strlen(request) - 1] = '\0'; // Convert last characther to NUL byte
                }
                write(fd1[2*0+WRITE], request, strlen(request)+1); // Write request to pipe, include NUL byte
                //write(fd[2*shortestline()+WRITE], request, SIZE);
                //queue_arr[i]++;
            }
        }
    }
    return 0;
}
