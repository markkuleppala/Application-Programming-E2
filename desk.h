#ifndef DESK_H
#define DESK_H

#include <stdio.h> /* scanf, printf, fgets */
#include <string.h> /* strtok */
#include <stdlib.h>  /* atof */
#include <pthread.h> /* pthread_create, phthread_mutex_* */
#include <math.h> /* log10, abs, floor */
#include <fcntl.h> /* fcntl */
#include <signal.h> /* signal */
#include <unistd.h>
#include <sys/mman.h> /* mmap */
#include <sys/types.h> /* gettid */

#define SIZE 1024 // Size for arrays

double getlastline(char *account); // Get the last line in bank file
void write_balance(char *account, double *value); // Write balance to file
double balance(char *number); // Read balance of file
double deposit(char *account, char *value); // Deposit to account
int withdraw(char *account, char *value); // Withdraw from account
double transfer(char *account1, char *account2, char *value); // Transfer value from account1 to account2
int shortestline(void); // Get the desk with shortest array
void *handlerequest(void *data); // Handle the request sent by master desk
void desk(int j, int *fd1, int *fd2, int *flag); // Desk main function

struct Data { // Data helper struct for desk-specific deposit and withdrawal
    char *readbuffer;
    int d;
    int w;
};
int n; // Number of desks
char read_buffer[SIZE]; // Read buffer array
pthread_mutex_t mutex_lock; // Mutex help variable
pid_t pid_logger; // Pid for logger



#endif // DESK_H
