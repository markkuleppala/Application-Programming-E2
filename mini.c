#include <stdio.h> /* scanf */
#include <string.h> /* strtok, strncmp */
#include <stdlib.h> /* exit */

#define BUFFER = 128

char** parser(void) {
    char **action;
    action = (char**)malloc(3*sizeof(char*));

    //char str[] = "l 1";
    char str[] = "w 1 123";
    //char str[] = "t 1 2 123";
    //char str[] = "d 1 234";
    if (strlen(str) == 0) { printf("No input"); exit(1); }
    int i;
    char* ptr = strtok(str, " ");
    int i_max = 3;
    if (strcmp(ptr, "l") == 0) { i_max = 2; }
    for (i = 0 ; i < i_max; i++) {
        action[i] = malloc(24);
        action[i] = ptr;
        //printf("+%s+", action[i]);
        ptr = strtok(NULL, " ");
    }
    action[i_max] = NULL;
    return action;
}

int main(void) {
    char** action;
    action = parser();
    printf("--%s--\n", action[0]);

}

/* “l 1”: give balance of account 1 “w 1 123”: withdraw 123 euros from account 1
“t 1 2 123”: transfer 123 euros from account 1 to account 2 “d 1 234”: deposit
234 euros to account 1 */