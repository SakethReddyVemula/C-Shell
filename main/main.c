#include "../utils/headers.h"

// Global Variables
char abs_path_home_dir[MAX_LEN] = {};
char user_name[MAX_USERNAME_LEN] = {'\0'};
bool isForegroundProcess = false;
bool shouldExit = false;

int main(){
    // get the absolute path of home directory of the shell (where it is invoked)
    getcwd(abs_path_home_dir, MAX_LEN);

    // Process signals
    signal(SIGINT, ctrl_C); // Interactive attention signal -> 2
    signal(SIGTSTP, ctrl_Z); // Keyboard stop signal -> 20
    
    while(shouldExit == false){
        isForegroundProcess = false;
        usleep(50000); // use sleep time so that, error printing such as perror/fprintf has enough time to print before next prompt
        // print initial prompt
        prompt();
        // Handle input commands
        char input[MAX_INPUT_LEN + 1] = {};
        if(fgets(input, MAX_INPUT_LEN + 1, stdin) == NULL){
            // handle later
            // ctrlDInterrupt();    
        }

        while(process_BGSIGCHLD() > 0){
            // handle bg processes in bgHandler.c
            // printf("Handling bg processes\n");
        }

        // check for a non blank character
        bool isNonBlank = false;
        int i = 0;
        for(int i = 0; input[i] != '\0'; i++){
            if(input[i] != ' ' || input[i] != '\t' || input[i] != '\n'){
                isNonBlank = true;
                break;
            }
        }
        if(isNonBlank == false){
            // all blank characters => invalid input inputed
            continue; // move onto next input; skip present
        }
        // non-blank character present => valid input inputed
        // since valid input, store a temporary copy of input for later log execution
        char* temp_input = (char*)calloc(sizeof(char), MAX_INPUT_LEN);
        strcpy(temp_input, input); // make copy of original input

        // Initialize a 2D-array of NULL characters
        char** inputCmnds = (char**)calloc(sizeof(char*), MAX_CMNDS);
        for(int i = 0; i < MAX_CMNDS; i++){
            inputCmnds[i] = (char*)calloc(sizeof(char), MAX_CMND_LEN);
        }

        // Parse the input
        int numCmnds = parseInput(input, inputCmnds);

        // print parsed commands
        // for(int i = 0; i < numCmnds; i++){
        //     printf("%d: %s\n", i + 1, inputCmnds[i]);
        // }

        time_t start_time = time(NULL); // need to include <time.h> header; keeps track of time to run a command
        for(int i = 0; i < numCmnds; i++){
            runCmnd(inputCmnds[i]);
        }
        
        // store the original input into log file
        add_to_log(temp_input);
        free(temp_input);

        // free input
        for(int i = 0; i < MAX_CMNDS; i++){
            free(inputCmnds[i]);
        }
        free(inputCmnds);

    }

    printf(BYEL "Logging out of Shell\n" CRESET);
}