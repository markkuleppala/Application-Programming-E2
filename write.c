#include <stdio.h> // prinft
#include <unistd.h> // sleep

// gcc -Wall write.c -o write && ./write

int main(void) {
    while (1) {
        printf("Writing on my own.\n");
        fflush(stdout);
        sleep(3);
    }
}