#ifndef THREADBANK_H
#define THREADBANK_H

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

#define INPUT_SIZE 1024
#define READ 0
#define WRITE 1

void sig_handler(int signo);
int main(int argc, char *argv[]);


int *queue_arr; // Desk specific queues
int flag = 0; // Flag for reporting the withdrawals and deposits
int n; // Number of desks

#endif // THREADBANK_H
