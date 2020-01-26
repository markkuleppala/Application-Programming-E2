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
int shortestline(void); // Should this be in threadbank.h?
struct Data;
void *handlerequest(void *data);
void desk(int j, int *fd1, int *fd2, int *flag);
pthread_mutex_t lock;
pid_t pid_logger;

//int *queue_arr; // Desk specific queues
//int flag = 0; // Flag for reporting the withdrawals and deposits
int n; // Number of desks
char read_buffer[SIZE];



#endif // DESK_H
