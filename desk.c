#include "desk.h"
#include "threadbank.h"
#include "lock.h"

double getlastline(char *account) { // Get balance - Check that file exists, if not, create an empty one
    double balance = 0; // Initialize open balance
    char *account_name = malloc(sizeof(char)*strlen(account)+6); // Allocating array
    sprintf(account_name, "%s.bank", account); // Writing id + .bank
    FILE *f; // Initilize file descriptor

    if (access(account_name, F_OK) == -1) { // Account file doesn't exist
        char init[SIZE]; // Initilize helper char array
        sprintf(init, "Creating a new account # %s\n", account); // Initilize text string to log creation of new account
        pid_logger = fork(); // Fork the process
        if (pid_logger < 0) { perror("Fork failed, skip logging."); } // Failed fork
        else if (pid_logger == 0) { execl("./logger", init, (char*) NULL); } // Child process, write account creation to log
        f = fopen(account_name, "a"); // Parent process, create file *.bank
        fprintf(f, "%.2f", balance); // Write initializing balance to the file *.bank
    }
    else { // Account file exists
        f = fopen(account_name, "r"); // Open existing account file
        char tmp[SIZE]; // Initialize helper char array
        while (!feof(f)) { // Find the last line
            fgets(tmp, SIZE, f);
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
    FILE *f = fopen(account_name, "ab+"); // Open the account file
    fprintf(f, "\n%.2f", *value); // Write in the new balance
    fclose(f); // Close the file

    char init[SIZE]; // Initilize helper char array
    sprintf(init, "Updating balance to %.2f in account # %s\n", *value, account); // Initilize text string to log creation of new account
    pid_logger = fork(); // Fork the process
    if (pid_logger < 0) { perror("Fork failed, skip logging."); } // Failed fork
    else if (pid_logger == 0) { execl("./logger", init, (char*) NULL); } // Child process, write account creation to log

    free(account_name);
}

double balance(char *number) {
    double balance = getlastline(number); // Get balance of account number
    printf("Balance of %s: %.2f.\n", number, balance); // Print the balance
    return balance;
}

double deposit(char *account, char *value) {
    char full_account[SIZE]; // Initialize full account name help array
    sprintf(full_account, "%s.bank", account); // Write full account file name to array
    int fd = lock(full_account, 2); // Write lock to account
    
    if (atof(value) >= 0) { // Check that the value is >= 0
        double new_balance = getlastline(account) + atof(value); // Calculate new balance
        write_balance(account, &new_balance); // Write new balance to account
        unlock(fd); // Unlock account

        char init[SIZE]; // Initilize helper char array
        sprintf(init, "Depositing %.2f to account # %s\n", atof(value), account); // Initilize text string to log creation of new account
        pid_logger = fork(); // Fork the process
        if (pid_logger < 0) { perror("Fork failed, skip logging."); } // Failed fork
        else if (pid_logger == 0) { execl("./logger", init, (char*) NULL); } // Child process, write account creation to log
        
        return new_balance;
    }
    else return -1;
}

int withdraw(char *account, char *value) {

    char full_account[SIZE]; // Initialize full account name help array
    sprintf(full_account, "%s.bank", account); // Write full account file name to array
    int fd = lock(full_account, 2); // Write lock to account

    double balance = getlastline(account); // Get the balance
    if (balance >= atof(value)) { // Check that balance is more than requested sum
        double new_balance = balance - atoi(value); // Calculate new balance
        write_balance(account, &new_balance); // Write new balance to file
        unlock(fd); // Unlock account

        char init[SIZE]; // Initilize helper char array
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

    char full_account1[SIZE]; // Initialize full account name help array
    char full_account2[SIZE]; // Initialize full account name help array
    sprintf(full_account1, "%s.bank", account1); // Write full account file name to array
    sprintf(full_account2, "%s.bank", account2); // Write full account file name to array
    int fd1 = lock(full_account1, 2); // Write lock to account1
    int fd2 = lock(full_account2, 2); // Write lock to account2

    double balance1 = getlastline(account1); // Get balance of account1
    if (balance1 >= atoi(value)) { // Check that balance is more than the requested value
        withdraw(account1, value); // Withdraw from account1
        deposit(account2, value); // Deposity to account1

        char init[SIZE]; // Initilize helper char array
        sprintf(init, "Transferring %.2f from account # %s to # %s\n", atof(value), account1, account2); // Initilize text string to log creation of new account
        pid_logger = fork(); // Fork the process
        if (pid_logger < 0) { perror("Fork failed, skip logging."); } // Failed fork
        else if (pid_logger == 0) { execl("./logger", init, (char*) NULL); } // Child process, write account creation to log
    }
    else {
        printf("Insufficient value on the account.\n");
        unlock(fd1); // Unlock account1
        unlock(fd2); // Unlock account2
        return -1;
    }
    unlock(fd1); // Unlock account1
    unlock(fd2); // Unlock account2
    return 1;
}


int shortestline(void) { // Get the shortest line of the desks
    int i;
    int location = 0;
    int min = queue_arr[0]; // Minimum position help variable
    size_t s = sizeof(&queue_arr)/sizeof(queue_arr[0]); // Length of the array
    for (i = 1; i < s; i++) { // Find the array with least tasks in queue
        if (queue_arr[i] < min) {
            min = queue_arr[i];
            location = i;
        }
    }
    return location; // Return the desk with shortest queue
}

void *handlerequest(void *data) { // Function for handling read requests
    pthread_mutex_lock(&mutex_lock); // Initialize mutex lock
    char *ptr = strtok(((struct Data *)data)->readbuffer, " "); // Split request by whitespaces
    int i_max; // Initialize upper limit for the for loop
    if (strcmp(ptr, "l") == 0) { i_max = 2; } // Balance request
    else if (strcmp(ptr, "t") == 0) { i_max = 4; } // Transfer request
    else { i_max = 3; } // Deposit or withdrawal
    char *action[i_max+1]; // Initialize array for saving the actions
    for (int i = 0 ; i < i_max; i++) { // Read the request
        if ((action[i] = ptr) != NULL){ // Check that string splitting was succesful
            ptr = strtok(NULL, " "); // Seek the for next action
        }
        else {
            fprintf(stderr, "Invalid request!\n");
            pthread_mutex_unlock(&mutex_lock); // Unlock the mutex lock
            return NULL;
        }
    }
    if (strcmp(action[0], "l") == 0) { // l - give balance
        if (balance(action[1])) { // Returns the balance
        }
    }
    else if (strcmp(action[0], "d") == 0) { // d - deposit
        if (deposit(action[1], action[2])) {
            ((struct Data *)data)->d = atoi(action[2]); // Save deposited value to the data structure
        }
    }
    else if (strcmp(action[0], "w") == 0) { // w - withdraw
        if (withdraw(action[1], action[2])) {
            ((struct Data *)data)->w = atoi(action[2]); // Save withdrawed value to the data structure
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
    pthread_mutex_unlock(&mutex_lock); // Unlock the mutex lock
    return NULL;
}

void desk(int j, int *fd1, int *fd2, int *flag) { // Desk function reading and handling requests
    close(fd1[2*j+WRITE]); // Close writing end of fd1
    close(fd2[2*j+READ]); // Close reading end of fd2
    pthread_t thread_id; // Initialize thread id
    int flag_local = 0; // Initialize local flag for reporting tracking
    int deposit_count = 0; // Initializing desk level deposit and withdraw counts
    int withdraw_count = 0;
    struct Data data; // Initializing data structure for passing variables between threads
    if (pthread_mutex_init(&mutex_lock, NULL) != 0) { perror("mutex init failed\n"); } // Invalid mutex lock initializing
    
    while(1) { // Get task from master thread and handle the queue
        if (*flag == 1) { // CTRL+T has been pressed, report the withdrawals and deposits
            if (flag_local == 0) { // Local flag to indicate if reporting has been done
                int arr[] = {deposit_count, withdraw_count}; // Initialize array for reporting d/w
                write(fd2[2*j+WRITE], arr, sizeof(arr)); // Report d/w to master desk
                flag_local = 1; // Raise local flag
            }
        }
        else if (*flag == 0 && read(fd1[2*j+READ], read_buffer, SIZE) > 0) { // Read task to queue from even pipe
            if (flag_local == 1) { flag_local = 0; } // Reset the local flag
            data.readbuffer = read_buffer; // Save read buffer to data structure
            data.d = 0; data.w = 0; // Initializing task level deposit and withdraw counts
            pthread_create(&thread_id, NULL, handlerequest, (void*)&data); // Create thread to handle request
            pthread_join(thread_id, NULL); // Waiting for the return of the task
            deposit_count += data.d; // Save deposit count to data structure
            withdraw_count += data.w; // Save withdraw count to data structure
            queue_arr[j]--; // Decrement the queue length
        }
    }
}
