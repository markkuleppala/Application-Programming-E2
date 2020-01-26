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

#define SIZE 1024

double getlastline(char *account);
void write_balance(char *account, double *value);
double balance(char *number);
double deposit(char *account, char *value);
int withdraw(char *account, char *value);
double transfer(char *account1, char *account2, char *value);
int shortestline(void);
void *handlerequest(void *data);
void desk(int j, int *fd1, int *fd2, int *flag);

struct Data { // Data helper struct for desk-specific deposit and withdrawal
    char *readbuffer;
    int d;
    int w;
};
int n; // Number of desks
char read_buffer[SIZE];
pthread_mutex_t lock;
pid_t pid_logger;



#endif // DESK_H
