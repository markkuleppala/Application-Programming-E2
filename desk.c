#include "desk.h"
#include "threadbank.h"

double getlastline(char *account) { // Get balance - Check that file exists, if not, create an empty one
    char tmp[1024];
    double balance = 0;
    char *account_name = malloc(sizeof(char)*strlen(account)+6); // Allocating array
    sprintf(account_name, "%s.bank", account); // Writing id + .bank
    FILE *f;
    if (access(account_name, F_OK) == -1) { // File doesn't exist
        printf("Creating a new account.\n");
        f = fopen(account_name, "a");
        fprintf(f, "%.2f", balance);
    }
    else {
        f = fopen(account_name, "r");
        while (!feof(f)) {
            fgets(tmp, 1024, f);
        }
        balance = atof(tmp);
    }
    fclose(f);
    free(account_name);
    return balance;
}

void write_balance(char *account, double *value) {
    char *account_name = malloc(sizeof(char)*strlen(account)+6); // Allocating array
    sprintf(account_name, "%s.bank", account); // Writing id + .bank
    FILE *f = fopen(account_name, "ab+");
    fprintf(f, "\n%.2f", *value);
    //printf("writing to %s value %.2f\n", account_name, *value);
    fclose(f);
    free(account_name);
}

double balance(char *number) {
    double balance = getlastline(number);
    printf("Balance of %s: %.2f.\n", number, balance);
    return balance;
}

double deposit(char *account, char *value) {

    // Write lock
    double new_balance = getlastline(account) + atof(value);
    printf("Deposit balance in %s: %.2f\n", account, new_balance);
    write_balance(account, &new_balance);
    // Write lock away
    return new_balance;
}

int withdraw(char *account, char *value) {

    // Write lock, use exec

    double balance = getlastline(account);
    if (balance >= atof(value)) {
        printf("Withdraw balance in %s: %.2f\n", account, balance);
        double new_balance = balance - atoi(value);
        // Write new balance to file
        write_balance(account, &new_balance);
    }
    else {
        printf("Insufficient value on the account.\n");
        return -1;
    }
    // Write lock away, use exec
    return 1;
}

double transfer(char *account1, char *account2, char *value) {
    // Write locks (and read locks), use exec
    double balance1 = getlastline(account1);
    if (balance1 >= atoi(value)) {
        withdraw(account1, value);
        deposit(account2, value);
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

// Data helper struct for desk-specific deposit and withdrawal
struct Data {
    char *readbuffer;
    int d;
    int w;
};

void *handlerequest(void *data) {
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
        if (transfer(action[1], action[2], action[3])) {
            printf("transfer from %s to %s of %s\n", action[1], action[2], action[3]);
        }
    }
    else { // Unknown request
        printf("Invalid request!\n");
    }
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
    //struct Data data_out; // Initializing data structure for passing deposit/withdrawal to master process
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
            //if ((strlen(read_buffer) > 0) && (read_buffer[strlen(read_buffer) - 1] == '\n')) { // What's this for?
            //    read_buffer[strlen(read_buffer) - 1] = '\0';
            //    printf("Read buffer: %s of %d\n", read_buffer, j);
            //}
            data.readbuffer = read_buffer;
            data.d = 0; data.w = 0; // Initializing task level deposit and withdraw counts
            pthread_create(&thread_id, NULL, handlerequest, (void*)&data);
            pthread_join(thread_id, NULL); // Waiting for the return of the task
            deposit_count += data.d;
            withdraw_count += data.w;

            //printf("queue length in %d: %d\n", j, queue_arr[j]);
            queue_arr[j]--; // Decrement the queue length
            //printf("queue length in %d: %d\n", j, queue_arr[j]);
        }
    }
}
