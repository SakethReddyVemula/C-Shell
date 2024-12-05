#include "../utils/headers.h"

/**
 * @brief Function to handle system calls
 * 
 * Returns child_pid
 */
int make_system_call(char* cmnd, bool is_background){
    // Note: Input "cmnd" is encoded using special characters for handling Quotes; So need to handle them here
    char* list_of_args[MAX_ARGS_IN_A_CMND] = {};
    int countArgs = 0;
    
    char storeCmnd[MAX_CMND_LEN + 1] = {}; // Store for later use
    for(int i = 0; cmnd[i] != '\0'; i++){
        storeCmnd[i] = cmnd[i];
    }
    char* token = strtok(cmnd, " \t\n");
    char* command_type = token;
    while(token){
        // printf("token: %s\n", token);
        // Replace special characters back to their original form (Decoding step) (Encoding was done while handling quotes)
        for(int i = 0; token[i] != '\0'; i++){
            // printf("%d ", i);
            switch(token[i]){
                case '\a': // ASCII Bell
                    token[i] = ' ';
                    break;
                case '\f': // ASCII Form Feed
                    token[i] = '\t';
                    break;
                case '\r': // ASCII Carriage Return
                    token[i] = '<';
                    break;
                case '\v': // ASCII Vertical Tab
                    token[i] = '>';
                    break;
                case '\b':
                    token[i] = '|';
                    break;
                case '\"':
                    // pop the char; Basically shift one index left
                    for(int j = i; token[j] != '\0'; j++){
                        token[j] = token[j + 1];
                    }
                    i--; // stay at the same index for next iteration
                    break;
                case '\'':
                    // pop the char; Basically shift one index left
                    for(int j = i; token[j] != '\0'; j++){
                        token[j] = token[j + 1];
                    }
                    i--; // stay at the same index for next iteration
                    break;
            }
        }
        // printf("\n");
        list_of_args[countArgs++] = token; // add token to list of args
        token = strtok(NULL, " \t\n"); // process next token
    }
    if(is_background == true && strcmp(list_of_args[countArgs - 1], "&") == 0){
        list_of_args[countArgs - 1] = NULL; // replace '&' argument with 'NULL'
        printf("replaced \"&\" argument with \"NULL\""); // Debugging
    }
    // printf("Hello from make_system_call\n");
    // make child process to handle background process
    int child_pid = fork();

    if(child_pid == -1){ // failed to fork
        fprintf(stderr, BRED "ERROR: sysCmnds: Failed to fork\n" CRESET);
        return -1; // -1 -> fork failed, Handle in processInput.c later
    } else if(child_pid == 0){
        // child process
        setpgid(0, 0); // (chatGPT)
        execvp(list_of_args[0], list_of_args); // Execute the system command (assuming its valid)

        // if execvp returns, it means there was an error
        fprintf(stderr, BRED "ERROR: Failed to execute command '%s'. Invalid command\n" CRESET, list_of_args[0]);
        exit(EXIT_FAILURE);
    } else if(child_pid > 0){
        // parent process
        if(is_background == true){
            // background process command. First add it to background process list.
            add_to_bg_processes_list(command_type, child_pid); // push to bg
            printf("%d\n", child_pid); // print the PID of bg process asa it completes execution.
            return child_pid; // return value PID of parent process i.e., PID of bg process that is 
        } else{
            // foreground process commmand. Shell will wait for process to complete and regain control afterwards ==> Waiting for child to finish executing
            int return_val = return_to_fg_method(command_type, child_pid);
            return return_val;
        }
    }

}