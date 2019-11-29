#include <stdio.h> /* scanf */
#include <string.h> /* strtok */
#include <stdlib.h> 
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

int getlastline(char *number) { // Get balance
    char line[1024]={0,};
    int balance = 0;
    char *account_number = malloc(sizeof(char)*strlen(number)+5); // Allocating array
    sprintf(account_number, "%s.bank", number); // Writing id + .bank
    if (access(account_number, F_OK) != -1) { // File exists
        FILE *f;
        f = fopen(account_number, "r");
        if (f != NULL) {
            while (fgets(line, 1024, f)) {} // Search the last line
            balance = atoi(line);
            fclose(f);
            free(account_number);
        }
        return balance;
    }
    printf("%s doesn't exist.\n", account_number);
    return -1;
}

int balance(char *number) {
    int balance = getlastline(number);
    printf("Balance of XX is: %d.\n", balance);
    return balance;
}

int deposit(char *number, char *amount) { // int *number, int *amount) { 

    // Write lock
    int new_balance = getlastline(number) + atoi(amount);
    // Append new balance to the end
    // Write lock away
    return new_balance;
}

int withdraw(char *number, char *amount) { // int *number, int *amount) { 

    // Write lock
    int balance = getlastline(number);
    if (balance >= atoi(amount)) {
        int new_balance = balance - atoi(amount);
        // Write new balance to file
    }
    else {
        printf("Insufficient amount on the account.\n");
        return 0;
    }
    // Write lock away
    return 1;
}

int transfer(char *account1, char *account2, char *amount) {
    // Write locks (and read locks)
    int balance1 = getlastline(account1);
    if (balance1 >= atoi(amount)) {
        withdraw(account1, amount);
        deposit(account2, amount);
    }
    else {
        printf("Insufficient amount on the account.\n");
        return -1;
    }
    // Write locks away
    return 1;
}

int handlerequest(char *request) { // added *
    char *ptr = strtok(request, " "); // removed &
    int i_max = 3;
    if (strcmp(ptr, "l") == 0) { i_max = 2; }
    char *action[i_max+1]; // malloc?
    for (int i = 0 ; i < i_max; i++) {
        action[i] = ptr;
        ptr = strtok(NULL, " ");
    }
    if (strcmp(action[0], "l") == 0) {
        int l = balance(action[1]);
        printf("l: %d\n", l);
        return 0; // return 1?
    }
    else if (strcmp(action[0], "d") == 0) {
        int d = deposit(action[1], action[2]);
        printf("d: %d\n", d);
        return atoi(action[2]);
    }
    else if (strcmp(action[0], "w") == 0) {
        int w = withdraw(action[1], action[2]);
        if (w >= 0) { return -atoi(action[2]); }
        else { return 0; }
    }
    else if (strcmp(action[0], "t") == 0) {
        int t = transfer(action[1], action[2], action[3]);
        if (t > 0) { return 1; }
        else { return 0; }
    }
    else { printf("Something wrent wrong in line 189\n"); }
    return -1;
}

void threadcounter(int i, int fd[]) {
    pthread_t thread_id;
    pid_t pid_c = -1;
    int status;
    char read_buffer[SIZE];
    double deposit_counter = 0;
    double withdraw_counter = 0;
    pthread_create(&thread_id, NULL, counter, (void*)&thread_id);
    //close(fd[2*i+WRITE]);
    printf("Counter number %d created with TID %d.\n", i, (int)thread_id);
    while(1) {
        if (read(fd[2*i], read_buffer, SIZE) > 0) {
            printf("readbuffer %s of %d", read_buffer, i);
            if ((strlen(read_buffer) > 0) && (read_buffer[strlen(read_buffer) - 1] == '\n')) {
            read_buffer[strlen(read_buffer) - 1] = '\0';
            printf("Read buffer: %s of %d\n", read_buffer, i);
            }
            pid_c = fork();
            if (pid_c < 0) { printf("Fork failed.\n"); }
            else if (pid_c == 0) { // Children
                printf("Children number: %d\n", i);
                int ret = handlerequest(read_buffer);
                printf("Return from handlerequest in %d: %i\n", i, ret);
                exit(0);
            }
            else { // Parent wait for termination
                wait(NULL);
                queue_arr[i]--;
            }        
        }
    }
}

int shortestline(void) {
    int i;
    int location = 0;
    int min = queue_arr[0];
    size_t s = sizeof(queue_arr)/sizeof(queue_arr[0]);
    for (i = 1; i < s; i++) {
        if (queue_arr[i] < min) {
            min = queue_arr[i];
            location = i;
        }
    }
    return location;
}

int main(void) {
    int n; char term;
    //char read_buffer[SIZE];
    //pthread_t thread_id;
    printf("Welcome to ThreadBank manager!\nHow many desk do we have open today? (1-N) ");
    if (scanf("%d%c", &n, &term) != 2 || term != '\n') { printf("Apparently we are closed today. Until tomorrow!\n"); exit(1); }
    else if (n < 1) { printf("Apparently we are closed today. Until tomorrow!\n"); exit(1); }
    else {
        pid_t pid_c = 0;
        queue_arr = calloc(n-1, sizeof *queue_arr); // vaikko n?
        if (!queue_arr) { printf("Allocating memory failed. Exiting.\n"); exit(1); }
        int fd[2*n]; // Initializing file describtors for pipes
        int i;
        printf("n %d\n", n);
        for (i = 0; i < n; i++) {
            pid_c = fork();
            if (pid_c < 0) { printf("Fork failed.\n"); }
            else if (pid_c == 0) { // Children
                threadcounter(i, fd); // Problem with async
            }
            pipe(&fd[2*i]);
            //close(fd[2*i+READ]);
        }
        printf("Great! The bank is open!\n");
        printf("The bank offers four different services:\nbalance (l),  withdraw (w), transfer (t), deposit (d).\n");

        //char *action[4];
        //char *ptr;
        char *request = malloc(INPUT_SIZE);
        if (request == NULL) { printf("No memory.\n"); }
        //queue_arr[0] = 2;
        //printf("%d\n", shortestline());


        while(signal(SIGINT, sig_handler) != SIG_ERR) {
            fgets(request, INPUT_SIZE, stdin);
            /*if ((strlen(request) > 0) && (request[strlen(request) - 1] == '\n')) {
                request[strlen(request) - 1] = '\0';
            }*/
            write(fd[2*shortestline()+WRITE], request, SIZE);
            queue_arr[i]++;
            /*
            ptr = strtok(request, " ");
            int i_max = 3;
            if (strcmp(ptr, "l") == 0) { i_max = 2; }
            for (i = 0 ; i < i_max; i++) {
                action[i] = ptr;
                ptr = strtok(NULL, " ");
            }
            if (strcmp(action[0], "l") == 0) {
                int l = balance(action[1]);
                printf("%d", l);
            }
            else if (strcmp(action[0], "d") == 0) {
                int d = deposit(action[1], action[2]);
                printf("%d", d);
            }
            else if (strcmp(action[0], "w") == 0) {
                int w = withdraw(action[1], action[2]);
            }
            else if (strcmp(action[0], "t") == 0) {
                int t = transfer(action[1], action[2], action[3]);
            }
            */
           //handlerequest(input);
        }
    }
    return 0;
}
