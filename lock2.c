#include <sys/file.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    int fd;
    fd = open("secret.txt", O_RDWR);
    fprintf(fd, "This is a new line.\n");
}