#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

// Copied from https://www.ict.griffith.edu.au/teaching/2501ICT/archive/guide/ipc/flock.html

int lock(char *filename, char type);
int unlock(int fd);
