#include "../utils/headers.h"

/**
 * @brief Function to get absolute path to .myshrc file
 */
char* get_myshrc_path(){
    char* path = (char*)calloc(sizeof(char), MAX_PATH_LEN + 1);
    strcat(path, abs_path_home_dir);
    strcat(path, "/");
    strcat(path, MYSHRC_FILE);
    return path;
}

/**
 * @brief Function to search and decode myshrc file
 */
int handleMyShrc(char* aliasToken){
    FILE* file = fopen(get_myshrc_path(), "r");
    if(!file){
        // Error / File doesn't exist
        fprintf(stderr, BRED "ERROR: file \".myshrc\" doesn't exist\n" CRESET);
        return 0;
    } else{
        long unsigned int len = 0;
        char* line = (char*)calloc(sizeof(char), MAX_CMND_LEN + 1);
        while(getline(&line, &len, file) > 0){
            // printf("searchToken: %s\n", line);
            char* token = NULL;
            token = strtok(line, " \t\n");
            // printf("token: %s\n", token);
            if(strcmp(token, "alias") == 0){
                token = strtok(NULL, " \t\n");
                if(strcmp(token, aliasToken) == 0){
                    // printf("Found match\n");
                    strtok(NULL, " \t\n"); // eat up "="
                    char* actualCmnd = strtok(NULL, ""); // Passing delimiter as empty string to get rest of the string
                    if(actualCmnd == NULL){
                        fprintf(stderr, BRED "ERROR: alias present but not defined\n" CRESET);
                        free(line);
                        return 0;
                    } else{
                        runCmnd(actualCmnd);
                        free(line);
                        return 1;
                    }
                }
            }
        }
        fclose(file);
        free(line);
        return 0;
    }
}