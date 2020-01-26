#include "lock.h"

int lock(char *filename, char type) {
    printf("%s\n", filename);
    struct flock fl;
    int fd;
    
    if (type == 1) 
        fl.l_type = F_RDLCK;
    else if (type == 2)
        fl.l_type = F_WRLCK;

    fl.l_whence = SEEK_SET; // Beginning of the file
    fl.l_start  = 0;        // Offset from l_whence
    fl.l_len    = 0;        // Length, 0 = to EOF
    fl.l_pid    = getpid(); // PID

    if ((fd = open(filename, O_RDWR)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("Trying to get lock...");

    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    printf("Got lock\n");

    return fd;
}

int unlock(int fd) {
    struct flock fl;

    fl.l_type   = F_UNLCK;  // Set to unlock same region
    fl.l_whence = SEEK_SET; // Beginning of the file
    fl.l_start  = 0;        // Offset from l_whence
    fl.l_len    = 0;        // Length, 0 = to EOF
    fl.l_pid    = getpid(); // PID

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    printf("Unlocked.\n");

    close(fd);
    return 1;
}
