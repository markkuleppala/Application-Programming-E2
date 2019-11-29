#include <pthread.h> // pthread_*
#include <stdio.h> //
#include <stdlib.h>  //
#include <sys/types.h>
#include <unistd.h>

/* Create a new thread
Create as daemon process
Listen to program passed
Exit when program exits 

Programs must be linked with "-lpthread".*/

// Compile and run with gcc -Wall listener.c -lpthread -o listener && ./listener

//void func();

void* func(void *ptr) {
    printf("thread %lu\n", (unsigned long) pthread_self());
    printf("pid id %d\n", getpid());
    pthread_exit(0);
}

int main(void) {
    pthread_t thread;
    pthread_create(&thread, NULL, func, NULL); // On success returns 0, otherwise error
    pthread_join(thread, NULL);
    return 0;
}