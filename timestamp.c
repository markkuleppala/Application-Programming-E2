// Copied from https://en.wikipedia.org/wiki/C_date_and_time_functions

#include "timestamp.h"

int timestamp(void)
{
    /* Obtain current time. */
    current_time = time(NULL);

    if (current_time == ((time_t)-1)) {
        (void) fprintf(stderr, "Failure to obtain the current time.\n");
        return 0;
    }

    /* Convert to local time format. */
    c_time_string = ctime(&current_time);

    if (c_time_string == NULL) {
        (void) fprintf(stderr, "Failure to convert the current time.\n");
        return 0;
    }

    return *c_time_string;
}


