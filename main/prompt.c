#include "../utils/headers.h"

// Sources: https://stackoverflow.com/questions/8953424/how-to-get-the-username-in-c-c-in-linux
void getUserName(char* userName){
    if(getlogin_r(userName, MAX_USERNAME_LEN + 1) != 0){
        perror(BRED "getlogin_r" CRESET);
        fprintf(stderr, RESET);
    }
}

// Source: https://www.geeksforgeeks.org/c-program-display-hostname-ip-address/
void getHostName(char* systemName){
    if(gethostname(systemName, MAX_SYSTEMNAME_LEN + 1) == -1){
        perror(BRED "gethostname" CRESET);
        fprintf(stderr, RESET);
    }
}

// Source: https://man7.org/linux/man-pages/man3/getcwd.3.html
void getCurrPath(char* currPath){
    if(getcwd(currPath, MAX_PATH_LEN + 1) == NULL){
        perror(BRED "return current directory" CRESET);
        fprintf(stderr, RESET);
    }
}

/**
 * @brief Get relative path give path to home directory and current directory path
 */
void getRelPath(char* relPath, char* currAbsPath, char* home_dir){
    int len_currAbsPath = strlen(currAbsPath);
    int len_home_dir = strlen(home_dir);

    if(strncmp(home_dir, currAbsPath, len_home_dir) == 0){
        // curr is inside home_dir
        int i = len_home_dir;
        int idx = 0;
        relPath[idx++] = '~'; // home directory
        while(currAbsPath[idx] != '\0'){
            relPath[idx++] = currAbsPath[i];
            i++;
        }
        relPath[idx] = '\0'; // terminate string
    } else{
        // curr is outside home_dir
        strcpy(relPath, currAbsPath);
    }
}

/**
 * @brief print the prompt in specified format
 */
void printPrompt(char* userName, char* systemName, char* relPath){
    printf("\n");
    printf(BLK "<" CRESET);
    printf(BGRN "%s" CRESET, userName);
    printf(BLK "@" CRESET);
    printf(BGRN "%s" CRESET, systemName);
    printf(BLK ":" CRESET);
    printf(BBLU "%s" CRESET, relPath);
    printf(BLK ">" CRESET);
    printf(" ");
    return;
}

/**
 * @brief Driver function for 'prompt'
 */
void prompt(){
    char userName[MAX_USERNAME_LEN + 1]; // Store username
    char systemName[MAX_SYSTEMNAME_LEN + 1]; // Store hostname
    char currAbsPath[MAX_PATH_LEN + 1]; // 
    char relPath[MAX_PATH_LEN + 1]; // Stores relative path from the directory from which shell is invoked

    getUserName(userName);
    getHostName(systemName);
    getCurrPath(currAbsPath);
    getRelPath(relPath, currAbsPath, abs_path_home_dir);
    printPrompt(userName, systemName, relPath);
    
}