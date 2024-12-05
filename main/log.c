#include "../utils/headers.h"

// typedef struct{
//     char** commands;
//     int count;
// } Log;

/**
 * @brief Initialize log history
 * @param log struct Log
 */
void init_log(Log* log){
    log->commands = (char**)calloc(sizeof(char*), MAX_CMNDS_IN_LOG + 1);
    log->count = 0;
}

/**
 * @brief Function to free() Log struct
 * @param log struct Log
 */
void free_log(Log* log){
    // Later... (Fix the error while adding more than 15)
    // loop only till last command not till MAX_CMNDS_IN_LOG (gives Segmentation fault);
    for(int i = 0; i <= MAX_CMNDS_IN_LOG; i++){
        // printf("freed %d\n", i);
        free(log->commands[i]);
    }
    free(log->commands);
}

/**
 * @brief Function to get path to Log file
 */
char* get_log_path(){
    // char path[MAX_PATH_LEN + 1];
    char* path = (char*)calloc(sizeof(char), MAX_PATH_LEN + 1);
    strcat(path, abs_path_home_dir);
    strcat(path, "/");
    strcat(path, LOG_FILE);
    return path;
}

/**
 * @brief load information from LOG_FILE into struct Log
 * @param log Empty struct Log
 */
void load_log(Log* log){
    FILE* file = fopen(get_log_path(), "r");
    if(!file){
        // Error / File doesn't exist
        // printf("Creating new log file\n");
        int fd = open(get_log_path(), O_CREAT, 0644);
        close(fd);
        file = fopen(get_log_path(), "r");
    }
    long unsigned int len = 0;
    while(getline(&(log->commands[log->count]), &len, file) > 0){
        log->count++;
    }
    fclose(file);
}

/**
 * @brief Function to save logs
 * @param log struct Log
 */
void save_log(Log* log){
    FILE* file = fopen(get_log_path(), "w");
    if(!file){
        // Error
        fprintf(stderr, BRED "ERROR: Unable to open or create Log file\n" CRESET);
        return;
    } else{
        int start = (log->count > MAX_CMNDS_IN_LOG) ? 1 : 0;
        for(int i = start; i < log->count; i++){
            fprintf(file, "%s", log->commands[i]);
        }
        fclose(file);
    }
}

/**
 * @brief Handles log command for different cases
 * 
 * Three (four) Cases:
 * 1. log (printing log)
 * 2. log execute <number>
 * 3. log purge (cleaning log)
 * 4. Other cases (Erroneous)
 * @param token Next string "log". Retrieved using strtok() command
 */
void handleLog(char* token){
    Log log; // Declare struct for log
    init_log(&log); // Intialize log
    load_log(&log); // Load log from LOG_FILE
    if(token == NULL){
        // print the log
        printf("printing the log content %d\n", log.count);
        for(int i = 0; i < (&log)->count; i++){
            printf("%d\t%s", i + 1, (&log)->commands[i]);
        }
    } else if(strcmp(token, "execute") == 0){
        // handle log execute case
        token = strtok(NULL, " \t\n");
        if(token == NULL){
            // Invalid case
            fprintf(stderr, BRED "ERROR: Missing necessary argument \"Index\" after log execute.\n" CRESET);
            return;
        } else{
            int index = atoi(token); // convert string token into integer index
            printf("log execute index: %d\n", index);
            if(index > 0 && index <= log.count){
                char* cmnd = strdup(log.commands[index - 1]);
                char* newline = strchr(cmnd, '\n');
                if(newline){
                    *newline = '\0';
                }
                printf("Command executing: %s\n", cmnd);
                // handle command
                char** inputCmnds = (char**)calloc(sizeof(char*), MAX_CMNDS);
                for(int i = 0; i < MAX_CMNDS; i++){
                    inputCmnds[i] = (char*)calloc(sizeof(char), MAX_CMND_LEN);
                }
                // parse as usual
                int numCmnds = parseInput(cmnd, inputCmnds);
                // loop through and run each command sequentially
                for(int i = 0; i < numCmnds; i++){
                    runCmnd(inputCmnds[i]);
                }

                for(int i = 0; i < MAX_CMNDS; i++){
                    free(inputCmnds[i]);
                }
                free(inputCmnds);
            } else{
                fprintf(stderr, BRED "ERROR: Invalid index after log execute\n" CRESET);
                return;
            }
        }
    } else if(strcmp(token, "purge") == 0){
        // handle log purge case -> delete and create log file -> equivalent to erasing
        remove(get_log_path());
        int fd = open(get_log_path(), O_CREAT, 0644);
        close(fd);
    } else{
        // Erroneous log command
        fprintf(stderr, BRED "ERROR: Invalid argument to log command. Should be among NULL, execute or purge.\n" CRESET);
        return;
    }
    free_log(&log);
}

/**
 * @brief Function to check whether the input is valid or not
 */
bool is_valid_input(char* input){
    for(int i = 0; input[i] != '\0'; i++){
        if(input[i] != ' ' && input[i] != '\t' && input[i] != '\n'){
            return true;
        }
    }
    return false;
}

/**
 * @brief Function to add a new command to log
 * @param token Raw input given to shell as string
 */
void add_to_log(char* input){
    char* isInputContainsLog = strstr(input, "log");
    if(isInputContainsLog){
        return;
    }

    // printf("Adding to log: %s\n", input);
    if(!is_valid_input(input)){
        return;
    }
    Log log;
    init_log(&log);
    load_log(&log);


    // If input is same as last command in log, don't add it to log again
    if(log.count > 0 && strcmp(log.commands[log.count - 1], input) == 0){
        free_log(&log);
        return;
    }

    FILE* log_file = fopen(get_log_path(), "w");
    if(log_file == NULL){
        fprintf(stderr, BRED "ERROR: open/create log file\n" CRESET);
        return;
    }

    // If already 15 commands are present, remove first and append new cmnd at last
    if(log.count == MAX_CMNDS_IN_LOG){
        for(int i = 0; i < MAX_CMNDS_IN_LOG; i++){
            log.commands[i] = log.commands[i + 1];
        }
        log.count--;
    }

    // add new command to log
    log.commands[log.count++] = strdup(input);

    save_log(&log);

    // for(int i = 0; i < log.count; i++){
    //     printf("ID: %d\tCmnd: %s\n", i, log.commands[i]);
    // }

    // free_log(&log); // Giving errors: free(): invalid size, corrupted size... (Fix this Later...)
    // printf("Free-ed all logs\n");
}

