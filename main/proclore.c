#include "../utils/headers.h"

/**
 * @brief Function to get the PID from token or use the current PID
 * @param token PID of procedss which we need info
 */
int get_pid(char* token){
    if(!token){
        return getpid();
    } else{
        return atoi(token);
    }
}

/**
 * @brief Function to read process information from /proc/[pid]/stat
 * 
 * Sources: 
 * 
 * getting pid: https://stackoverflow.com/questions/20999255/get-the-status-of-a-specific-pid
 *          
 * Content inside stat: https://metacpan.org/pod/Proc::Stat#:~:text=Contents%20of%20%2Fproc%2F%5Bpid%5D%2Fstat%20from%20proc(5),-pid%20%25d&text=(1)%20The%20process%20ID.&text=(2)%20The%20filename%20of%20the,the%20executable%20is%20swapped%20out.
 */
int read_process_info(int pid, char* status, int* pgrp, int* tpgid, unsigned long* vsize){
    char stats_path[MAX_PATH_LEN];
    snprintf(stats_path, sizeof(stats_path), "/proc/%d/stat", pid);

    FILE* filePtr = fopen(stats_path, "r");

    if(!filePtr){
        return -1;
    } else{
        // Specifing before format specifier '*' doesn't store the value
        fscanf(filePtr, "%*d %*s %s %*s %d %*s %*s %d %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %lu", status, pgrp, tpgid, vsize);
    }
    fclose(filePtr);
    return 0;
}

/**
 * @brief Function to check if the process is in the foreground
 */
void check_foreground_status(char* status, int pgrp, int tpgid){
    if (pgrp == tpgid) {
        status[1] = '+';
        status[2] = '\0';
    }
}

/**
 * @brief to read the executable path from /proc/[pid]/exe
 */
int read_executable_path(int pid, char* exec_path_input){
    char exec_path[MAX_PATH_LEN];
    snprintf(exec_path, sizeof(exec_path), "/proc/%d/exe", pid);

    int returnVal = readlink(exec_path, exec_path_input, MAX_PATH_LEN);
    return returnVal == -1 ? -1 : 0;
}

/**
 * @brief Function to obtain information regarding a process
 * 
 * sample output:
 * 
 * pid: 210
 * 
 * process state: R+
 * 
 * Process Group: 210
 * 
 * Virtual memory: 167142
 * 
 * executable path: ~/a.out
 * @param token PID of the process of which information is needed
 */
void proclore(char* token){
    // pid
    int pid = get_pid(token);

    // Use pgrp and tpgid combo to check whether a process in Foreground or Background
    int pgrp, tpgid; // pgrp -> process group ID, tpgid -> Terminal Group ID
    unsigned long vsize;
    char status[3] = {};
    char exec_path[MAX_PATH_LEN] = {};
    char resolved_path[MAX_PATH_LEN] = {};

    if(read_process_info(pid, status, &pgrp, &tpgid, &vsize) == -1){
        fprintf(stderr, BRED "ERROR: Invalid PID passed to proclore\n" CRESET);
        return;
    }
    
    // If both pgrp an tpgid are same, then process is foreground, else background
    check_foreground_status(status, pgrp, tpgid);

    if(read_executable_path(pid, exec_path) == -1){
        perror("readlink failed\n");
        return;
    }

    // get path relative to home directory of C-Shell
    getRelPath(resolved_path, exec_path, abs_path_home_dir);

    printf("PID : %d\n", pid);
    printf("Process Status : %s\n", status);
    printf("Process Group : %d\n", pgrp);
    printf("Virtual Memory : %lu bytes\n", vsize);
    printf("Executable Path : %s\n", resolved_path);

}