#include "../utils/headers.h"

bool a_dir_visited = false;
char currDir[MAX_PATH_LEN];
char prevDir[MAX_PATH_LEN];

/*  Function to change directory using chdir in routine    */
void cd(char* target_path){
    int retVal = chdir(target_path);
    if(retVal == -1){
        fprintf(stderr, BRED "ERROR: hop failed for path \"%s\". Ensure valid directory exist with necessary permissions\n" CRESET, target_path);
        return;
    } else{
        strcpy(prevDir, currDir);
        getcwd(currDir, MAX_PATH_LEN);
        a_dir_visited = true;
        // printf("%s\n", currDir); // This prints absolute path wrt original linux shell source code
        // Print relative path to home directory of C-shell instead of absolute path from linux shell
        char relPath[MAX_PATH_LEN + 1];
        getRelPath(relPath, currDir, abs_path_home_dir);
        printf("%s\n", relPath);
    }
}

/*  Handle case of hop to home directory of the shell   */
void handleHomeDir(char* token){
    char finalPath[MAX_PATH_LEN] = {};
    strcat(finalPath, abs_path_home_dir);
    strcat(finalPath, token + 1); // ignore '~' and copy anything remaining
    cd(finalPath);
}

/*  Handle case of hop previous directory   */
void handlePreviousDir(char* token){
    if(a_dir_visited == true){
        cd(prevDir);
    } else{
        printf(BGRN "OLDPWD not set\n" CRESET); // prevDir hasn't been set
    }
}

/*  Process each token in hop command sequentially   */
void processToken(char* token){
    if(token[0] == '-'){
        handlePreviousDir(token);
    } else if(token[0] == '~'){
        handleHomeDir(token);
    } else{
        cd(token);
    }
}

/*  Function to execute hop */
void hop(char* token){
    // get current directory using getcwd()
    getcwd(currDir, MAX_PATH_LEN);
    if(token == NULL){
        cd(abs_path_home_dir);
    } else{
        while(token != NULL){
            processToken(token);
            token = strtok(NULL, " \t\n");
        }
    }
}