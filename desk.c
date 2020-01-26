#include "desk.h"
#include "threadbank.h"

double getlastline(char *account) { // Get balance - Check that file exists, if not, create an empty one
    double balance = 0; // Initialize open balance
    char *account_name = malloc(sizeof(char)*strlen(account)+6); // Allocating array
    sprintf(account_name, "%s.bank", account); // Writing id + .bank
    FILE *f; // Initilize file descriptor

    if (access(account_name, F_OK) == -1) { // Account file doesn't exist
        char init[1024]; // Initilize helper char array
        sprintf(init, "Creating a new account # %s\n", account); // Initilize text string to log creation of new account
        pid_logger = fork(); // Fork the process
        if (pid_logger < 0) { perror("Fork failed, skip logging."); } // Failed fork
        else if (pid_logger == 0) { execl("./logger", init, (char*) NULL); } // Child process, write account creation to log
        f = fopen(account_name, "a"); // Parent process, create file *.bank
        fprintf(f, "%.2f", balance); // Write initializing balance to the file *.bank
    }
    else { // Account file exists
        f = fopen(account_name, "r"); // Open existing account file
        char tmp[1024]; // Initialize helper char array
        while (!feof(f)) { // Find the last line
            fgets(tmp, 1024, f);
        }
        balance = atof(tmp); // Read the balance
    }
    fclose(f); // Close the filehandler
    free(account_name); // Free the allocated filename
    return balance; // Return the latest balance
}

void write_balance(char *account, double *value) {
    char *account_name = malloc(sizeof(char)*strlen(account)+6); // Allocating array
    sprintf(account_name, "%s.bank", account); // Writing id + .bank
    FILE *f = fopen(account_name, "ab+");
    fprintf(f, "\n%.2f", *value);
    fclose(f);

    char init[1024]; // Initilize helper char array
    sprintf(init, "Updating balance to %.2f in account # %s\n", *value, account); // Initilize text string to log creation of new account
    pid_logger = fork(); // Fork the process
    if (pid_logger < 0) { perror("Fork failed, skip logging."); } // Failed fork
    else if (pid_logger == 0) { execl("./logger", init, (char*) NULL); } // Child process, write account creation to log

    free(account_name);
}

double balance(char *number) {
    double balance = getlastline(number);
    printf("Balance of %s: %.2f.\n", number, balance);
    return balance;
}

double deposit(char *account, char *value) {

    // Write lock
    if (atof(value) >= 0) {
        double new_balance = getlastline(account) + atof(value);
        printf("Deposit balance in %s: %.2f\n", account, new_balance);
        write_balance(account, &new_balance);
        // Write lock away

        char init[1024]; // Initilize helper char array
        sprintf(init, "Depositing %.2f to account # %s\n", atof(value), account); // Initilize text string to log creation of new account
        pid_logger = fork(); // Fork the process
        if (pid_logger < 0) { perror("Fork failed, skip logging."); } // Failed fork
        else if (pid_logger == 0) { execl("./logger", init, (char*) NULL); } // Child process, write account creation to log
        
        return new_balance;
    }
    else return -1;
}

int withdraw(char *account, char *value) {

    // Write lock, use exec

    double balance = getlastline(account);
    if (balance >= atof(value)) {
        double new_balance = balance - atoi(value);
        printf("Withdraw balance in %s: %.2f\n", account, new_balance);
        // Write new balance to file
        write_balance(account, &new_balance);
        // Write lock away, use exec

        char init[1024]; // Initilize helper char array
        sprintf(init, "Withdrawing %.2f from account # %s\n", atof(value), account); // Initilize text string to log creation of new account
        pid_logger = fork(); // Fork the process
        if (pid_logger < 0) { perror("Fork failed, skip logging."); } // Failed fork
        else if (pid_logger == 0) { execl("./logger", init, (char*) NULL); } // Child process, write account creation to log

    }
    else {
        printf("Insufficient value on the account.\n");
        return -1;
    }
    
    return 1;
}

double transfer(char *account1, char *account2, char *value) {
    // Write locks (and read locks), use exec
    double balance1 = getlastline(account1);
    if (balance1 >= atoi(value)) {
        withdraw(account1, value);
        deposit(account2, value);

        char init[1024]; // Initilize helper char array
        sprintf(init, "Transferring %.2f from account # %s to # %s\n", atof(value), account1, account2); // Initilize text string to log creation of new account
        pid_logger = fork(); // Fork the process
        if (pid_logger < 0) { perror("Fork failed, skip logging."); } // Failed fork
        else if (pid_logger == 0) { execl("./logger", init, (char*) NULL); } // Child process, write account creation to log
    }
    else {
        printf("Insufficient value on the account.\n");
        return -1;
    }
    // Write locks away, use exec
    return 1;
}

// Get the shortest line of the desks
int shortestline(void) {
    int i;
    int location = 0;
    int min = queue_arr[0];
    size_t s = sizeof(&queue_arr)/sizeof(queue_arr[0]);
    for (i = 1; i < s; i++) {
        if (queue_arr[i] < min) {
            min = queue_arr[i];
            location = i;
        }
    }
    return location;
}

void *handlerequest(void *data) {
    pthread_mutex_lock(&lock);
    char *ptr = strtok(((struct Data *)data)->readbuffer, " ");
    int i_max;
    if (strcmp(ptr, "l") == 0) { i_max = 2; }
    else if (strcmp(ptr, "t") == 0) { i_max = 4; }
    //else if (strcmp(ptr, "report") == 0) { i_max = 1; }
    else { i_max = 3; }
    char *action[i_max+1]; // malloc?
    for (int i = 0 ; i < i_max; i++) { // Read the request
        if ((action[i] = ptr) != NULL){
            ptr = strtok(NULL, " ");
        }
        else {
            fprintf(stderr, "Invalid request!\n");
            pthread_mutex_unlock(&lock);
            return NULL;
        }
    }
    if (strcmp(action[0], "l") == 0) { // l - give balance
        if (balance(action[1])) { // Returns the balance
            //printf("balance of %s checked\n", action[1]);
        }
    }
    else if (strcmp(action[0], "d") == 0) { // d - deposit
        if (deposit(action[1], action[2])) {
            ((struct Data *)data)->d = atoi(action[2]);
        }
    }
    else if (strcmp(action[0], "w") == 0) { // w - withdraw
        if (withdraw(action[1], action[2])) {
            ((struct Data *)data)->w = atoi(action[2]);
        }
    }
    else if (strcmp(action[0], "t") == 0) { // t - transfer
        if (transfer(action[1], action[2], action[3]) == 1) {
            printf("Transfered %s from %s to %s\n", action[3], action[1], action[2]); // Do not print this is action not successful
        }
    }
    else { // Unknown request
        printf("Invalid request!\n");
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

void desk(int j, int *fd1, int *fd2, int *flag) {
    close(fd1[2*j+WRITE]); // Close writing end of fd1
    close(fd2[2*j+READ]); // Close reading end of fd2
    pthread_t thread_id;
    int flag_local = 0;
    int deposit_count = 0; // Initializing desk level deposit and withdraw counts
    int withdraw_count = 0;
    struct Data data; // Initializing data structure for passing variables between threads
    
    if (pthread_mutex_init(&lock, NULL) != 0) {
        perror("mutex init failed\n");
    }
    
    
    while(1) { // Get task from master thread and handle the queue
        if (*flag == 1) {
            if (flag_local == 0) {
                int arr[] = {deposit_count, withdraw_count};
                write(fd2[2*j+WRITE], arr, sizeof(arr));
                flag_local = 1;
            }
        }
        else if (*flag == 0 && read(fd1[2*j+READ], read_buffer, SIZE) > 0) { // Read task to queue from even pipe
            if (flag_local == 1) { flag_local = 0; }
            data.readbuffer = read_buffer;
            data.d = 0; data.w = 0; // Initializing task level deposit and withdraw counts
            pthread_create(&thread_id, NULL, handlerequest, (void*)&data);
            pthread_join(thread_id, NULL); // Waiting for the return of the task
            deposit_count += data.d;
            withdraw_count += data.w;
            queue_arr[j]--; // Decrement the queue length
        }
    }
}
