#ifndef THREADBANK_H_
#define THREADBANK_H_

#include <stdio.h> /* scanf, printf, fgets */
#include <string.h> /* strtok */
#include <stdlib.h>  /* atof */
#include <pthread.h> /* pthread_create */
#include <math.h> /* log10, abs, floor */
#include <fcntl.h> /* fcntl */
#include <signal.h> /* signal */
#include <unistd.h>
#include <sys/mman.h> /* mmap */
#include <sys/types.h> /* gettid */

#define SIZE 1024
#define INPUT_SIZE 1024
#define READ 0
#define WRITE 1

void sig_handler(int signo);
double getlastline(char *account);
void write_balance(char *account, double *value);
double balance(char *number);
double deposit(char *account, char *value);
int withdraw(char *account, char *value);
double transfer(char *account1, char *account2, char *value);
int shortestline(void);
struct Data;
void *handlerequest(void *data);
void desk(int j, int fd1[], int fd2[]);
int main(int argc, char *argv[]);


int *queue_arr; // Desk specific queues
int flag; // Flag for reporting the withdrawals and deposits
int n; // Number of desks

#endif // THREADBANK_H_
