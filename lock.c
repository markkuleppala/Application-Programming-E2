#include "lock.h"

int lock(char *filename, char type) {

    int fd; // File descriptor
    
    if (type == 1) // Read lock
        fl.l_type = F_RDLCK;
    else if (type == 2) // Write lock
        fl.l_type = F_WRLCK;

    fl.l_whence = SEEK_SET; // Beginning of the file
    fl.l_start  = 0;        // Offset from l_whence
    fl.l_len    = 0;        // Length, 0 = to EOF
    fl.l_pid    = getpid(); // PID

    FILE *f = fopen(filename, "ab+"); fclose(f); // Open and close the account file to create it if not existing

    if ((fd = open(filename, O_RDWR)) == -1) { // Open the file
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (fcntl(fd, F_SETLKW, &fl) == -1) { // Set lock to file
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    return fd;
}

int unlock(int fd) {

    fl.l_type   = F_UNLCK;  // Set to unlock same region
    fl.l_whence = SEEK_SET; // Beginning of the file
    fl.l_start  = 0;        // Offset from l_whence
    fl.l_len    = 0;        // Length, 0 = to EOF
    fl.l_pid    = getpid(); // PID

    if (fcntl(fd, F_SETLK, &fl) == -1) { // Unlock the file
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    close(fd); // Close the file handler
    return 1;
}
