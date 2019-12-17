#ifndef THREADBANK_H_
#define THREADBANK_H_

#include <stdio.h> /* scanf, printf, fgets */
#include <string.h> /* strtok */
#include <stdlib.h>  /* atof */
#include <pthread.h> /* pthread_create */
#include <sys/types.h> /* gettid */
#include <math.h> /* log10, abs, floor */
#include <fcntl.h> /* fcntl */
#include <signal.h> /* signal */
#include <unistd.h>


#define SIZE 1024
#define INPUT_SIZE 1024
#define READ 0
#define WRITE 1


//int queue_arr; // vaikko char
int *queue_arr;
int flag;

#endif // THREADBANK_H_
