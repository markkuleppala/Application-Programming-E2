#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main() {
    int fd[2];
    pipe(fd);
    // You must read from fd[0] and write from fd[1]
    printf("Reading from %d, writing to %d\n", fd[0], fd[1]);

    pid_t p = fork();
    if (p == 0) {
        /* Child */
        close(STDOUT_FILENO);
        dup(fd[1]);
        printf("Hi Daddy!");
        //write(1,"Hi Child!",9);

        wait(NULL);
    } else {
        char buf;
        int bytesread;
        // read one byte at a time.
        while ((bytesread = read(fd[0], &buf, 1)) > 0) {
            putchar(buf);
            fflush(stdout);
        }
    }
    return 0;
}
