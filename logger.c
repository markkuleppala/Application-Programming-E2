#include "logger.h"
#include "lock.h"

char* timestamp(void) { // Copied from https://en.wikipedia.org/wiki/C_date_and_time_functions
    current_time = time(NULL); // Obtain current time

    if (current_time == ((time_t)-1)) {
        fprintf(stderr, "Failure to obtain the current time.\n");
        exit(EXIT_FAILURE);
    }
    c_time_string = ctime(&current_time); // Convert to local time format

    if (c_time_string == NULL) {
        fprintf(stderr, "Failure to convert the current time.\n");
        exit(EXIT_FAILURE);
    }
    c_time_string[strlen(c_time_string)-1] = '\0'; // Remove the newline initialized by ctime
    return c_time_string;
}

int main(int argc, char *argv[]) {
    if (access(LOG_FILE, F_OK) == -1) { // Log file doesn't exist
        //printf("%s logfile\n", LOG_FILE);
        FILE *f = fopen(LOG_FILE, "a");
        fputs("Welcome to ThreadBank manager!\n", f);
        fclose(f);
    }
    // exec write lock on LOG_FILE
    int fd = lock(LOG_FILE, 2);
    FILE *f = fopen(LOG_FILE, "a"); // Open file handler for the logfile
    if (!argv[0]) { // Check if the argument is empty
        perror("Invalid log");
    }
    char* time_now = timestamp(); // Get the timestamp
    fprintf(f, "%s: %s", time_now, argv[0]); // Write timestamp and log text to the logfile
    fclose(f); // Close the file handler
    unlock(fd);


}
