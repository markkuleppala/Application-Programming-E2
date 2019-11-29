#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {

int fd;
fd = open("secret.txt", O_RDWR | O_CREAT | O_EXLOCK); /* not 100% sure if O_EXCL needed */

sleep(100000000);


}