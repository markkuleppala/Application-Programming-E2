#ifndef LOGGER_H
#define LOGGER_H

#include <stdlib.h> /* EXIT_FAILURE */
#include <stdio.h> /* FILE, fprintf */
#include <string.h> /* strlen */
#include <time.h> /* current_time, ctime */

#define LOG_FILE "transaction_log.txt"

char* timestamp(void);
int main(int argc, char *argv[]);

time_t current_time;
char* c_time_string;

#endif // LOGGER_H
