#include "logger.h"


int main(int argc, char *argv[]) {
    // exec write lock on LOG_FILE
    FILE *f = fopen(LOG_FILE, "a");
    if (!argv[0]) {
        perror("Invalid log");
    }
    fprintf(f, "%s", argv[0]);
    fclose(f);


}