#ifndef THREADBANK_H
#define THREADBANK_H

#include <stdio.h> /* scanf, printf, fgets */
#include <string.h> /* strtok, memset */
#include <stdlib.h>  /* atof */
#include <pthread.h> /* pthread_create, pthread_mutex_* */
#include <math.h> /* log10, abs, floor */
#include <fcntl.h> /* fcntl */
#include <signal.h> /* signal, sigaction */
#include <unistd.h>
#include <sys/mman.h> /* mmap */
#include <sys/types.h> /* gettid */

#define INPUT_SIZE 1024 
#define READ 0
#define WRITE 1

void sig_handler(int signo); // Handle signals
void master_desk(void); // Master desk function
int main(int argc, char *argv[]); // Main function initializing the bank

int *queue_arr; // Desk specific queues
int *flag; // Flag for reporting the withdrawals and deposits
int n; // Number of desks
int *fd1; // Master to desk, d1[0] to read and fd1[1] to write
int *fd2; // Desk to master
char *request; // Array for requests
pid_t pid_p; // Parent PID
int i;
int deposit_master; // Variable for total deposit count
int withdraw_master; // Variable for total withdrawal count

#endif // THREADBANK_H
