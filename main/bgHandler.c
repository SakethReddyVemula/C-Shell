#include "../utils/headers.h"

// For reference:

// For each process, we need to store PID and command name
// typedef struct {
    // int PID;
    // char* cmnd_name;
// } ProcessInfo;

// Global Variables:
// Maintain a list of process commandss
int count_bg_processes = 0;
ProcessInfo* bg_processes[MAX_BG_PROCESSES] = {NULL};
int status = 0; // Keep track of status of child process in case of handling background process handling.

/**
 * @brief Helper function which sorts the list of background processes
 */
void sort_bg_processes(){
    qsort(bg_processes, count_bg_processes, sizeof(ProcessInfo*), custom_cmp);
}


/**
 * @brief Function to compare two processes by their command names.
 * 
 * Helps in sorting the 'bg_processes' array alphabetically by command name.
 * 
 * Returns 1 if first argument is NULL, -1 if second argument is NULL. Otherwise return the value returned by strcmp
 */
int custom_cmp(const void* A, const void* B){
    if(A == NULL || B == NULL){
        fprintf(stderr, BRED "ERROR: Null pointers passed to custom_cmp function\n" CRESET);
        return 0;
    }
    ProcessInfo* x = *((ProcessInfo**) A); // Typecast then de-reference
    ProcessInfo* y = *((ProcessInfo**) B); // Typecast then de-reference
    if(x == NULL){
        return 1;
    } else if(y == NULL){
        return -1;
    } else{
        // if none of them are NULL, then compare their command names (based on alphabetical order)
        return strcmp(x->cmnd_name, y->cmnd_name);
    }
}

/**
 * @brief Function to check whether process with PID 'child_pid' is present in the background processes list
 */
bool check_proc_in_bg_proceses(int child_pid){
    for(int i = 0; i < count_bg_processes; i++){
        if(bg_processes[i] != NULL && bg_processes[i]->PID == child_pid){
            return true;
        }
    }
    fprintf(stderr, BRED "Warning: Process with PID %d not found in background process list\n" CRESET, child_pid);
    return false;
}

/**
 * @brief Remove process with PID 'child_pid' from 'bg_processes' array list.
 */
void remove_process(int child_pid){
    for(int i = 0; i < count_bg_processes; i++){
        if(bg_processes[i] != NULL && bg_processes[i]->PID == child_pid){
            free_process_info(bg_processes[i]);
            bg_processes[i] = NULL;
            sort_bg_processes();
            count_bg_processes--;
            break;
        }
    }
}

/**
 * @brief Helper function to create a struct of process for entry in bg_processes list later
 */
ProcessInfo* create_process_info(int pid, char* cmnd_name){
    if(cmnd_name == NULL){
        fprintf(stderr, BRED "ERROR: Null cmnd_name passed to create_process_info\n" CRESET);
        return NULL;
    }
    ProcessInfo* proc = (ProcessInfo*)calloc(sizeof(ProcessInfo), 1);
    if(proc == NULL){
        fprintf(stderr, BRED "ERROR: allocating memory for ProcessInfo\n" CRESET);
        return NULL;
    }
    proc->PID = pid;
    proc->cmnd_name = strdup(cmnd_name);
    if(proc->cmnd_name == NULL){
        fprintf(stderr, BRED "ERROR: duplicating command name\n" CRESET);
        return NULL;
    }
    return proc;
}

/**
 * @brief Helper function to free a process from bg list
 */
void free_process_info(ProcessInfo* proc){
    if(proc){
        free(proc->cmnd_name);
        free(proc);
    }
}

/**
 * @brief Add a new background process to the 'bg_processes' array
 * 
 * Stores the process ID and command line in new 'ProcessInfo' struct and inserts it into the first available slot in the array.
 */
void add_to_bg_processes_list(char* cmnd_name, int child_pid){
    ProcessInfo* proc = create_process_info(child_pid, cmnd_name);
    for(int i = 0; i < MAX_BG_PROCESSES; i++){
        // loop through each listed bg processes until a empty/NULL instance is found. Insert there.
        if(bg_processes[i] == NULL){
            bg_processes[i] = proc; // Store the process into list of BG processes
            count_bg_processes++; // increase the count of bg processes
            sort_bg_processes();
            return;
        }
    }
    fprintf(stderr, BRED "Unable to add process to background list. Check if size might be full.\n" CRESET);
    free_process_info(proc); // free
}

int process_BGSIGCHLD(){
    int child_pid = waitpid(-1, &status, WNOHANG);
    if(child_pid <= 0){
        return child_pid;
    } else{
        for(int i = 0; i < count_bg_processes; i++){
            if(bg_processes[i] != NULL && bg_processes[i]->PID == child_pid){
                bool is_normal_exit = WIFEXITED(status);
                if(is_normal_exit == false){
                    fprintf(stderr, BRED "ERROR: %s exited abnormally (%d)\n" CRESET, bg_processes[i]->cmnd_name, bg_processes[i]->PID);
                } else{
                    printf(GRN "%s exited normally (%d)\n" CRESET, bg_processes[i]->cmnd_name, bg_processes[i]->PID);                  
                }
                free_process_info(bg_processes[i]);
                bg_processes[i] = NULL;
                sort_bg_processes();
                count_bg_processes--;  
                break;
            }
        }
    }
    return child_pid;
}



// Print list of all the processes currently running that were spawned by your shell in lexicographical order.
// Format: [pid] : [command name] - [state]
// Process State: either "Running" or "Stopped"
void print_process_details(ProcessInfo* proc){
    if(proc == NULL){
        return;
    } else{
        char temp_status;
        
        int process_status = 0;
        char temp_buffer[MAX_PATH_LEN + 1];
        sprintf(temp_buffer, "/proc/%d/stat", proc->PID);

        FILE* filePtr = fopen(temp_buffer, "r"); // open as readonly
        if(filePtr == NULL){
            fprintf(stderr, BRED "ERROR: Process with process ID %d doesn't exist\n" CRESET, proc->PID);
            process_status = 0;
        } else{
            // open successful
            fscanf(filePtr, "%*d %*s %c", &temp_status); // skip others which are unnecessary
            fclose(filePtr);
            process_status = 1;
        }
        
        if(process_status == 1){
            printf("%d : %s - %s\n", proc->PID, proc->cmnd_name, temp_status == 'T' ? "Stopped" : "Running");
        } else{
            return;
        }
    }
}

/**
 * @brief Print all the processes that are spawned by the shell
 * 
 * Note:
 * 
 * Since, there can be only background processes that can run, simply prints all the background processes present.
 */
void activities(){
    if(count_bg_processes == 0){
        printf(BYEL "No current processes spawned by the shell\n" CRESET);
        return;
    } else{
        for(int i = 0; i < count_bg_processes; i++){
            print_process_details(bg_processes[i]);
        }
    }
}

// Change the state of a stopped background process to running (in the background)
void bg(char* token){
    if(token == NULL){
        fprintf(stderr, BRED "ERROR: Correct Usage: bg <pid>\n" CRESET);
        return;
    }
    int pid = atoi(token);
    if(check_proc_in_bg_proceses(pid) == false){
        fprintf(stderr, BRED "ERROR: Invalid <pid> passed to bg. No such background process is exist. Usage: bg <pid>\n" CRESET);
        return;
    }
    char temp_buffer[MAX_PATH_LEN + 1];
    sprintf(temp_buffer, "/proc/%d/stat", pid);

    FILE* filePtr = fopen(temp_buffer, "r"); // open as readonly
    if(filePtr == NULL){
        fprintf(stderr, BRED "ERROR: Process with PID %d doesn't exist. No such process found.\n" CRESET, pid);
        return;
    }
    char cmnd_name[MAX_CMND_LEN + 1] = {'\0'};
    fscanf(filePtr, "%*d %s %*c", cmnd_name);
    // printf("cmnd_name: %s\n", cmnd_name); // (sleep) -> parenthesis around the cmnd_name
    for(int i = 1; i < strlen(cmnd_name); i++){
        cmnd_name[i - 1] = cmnd_name[i];
    }
    cmnd_name[strlen(cmnd_name) - 2] = '\0';
    // printf("cmnd_name: %s\n", cmnd_name); // sleep -> without parenthesis
    fclose(filePtr); // work done -> close

    if(cmnd_name == NULL){
        return;
    }
    if(kill(pid, SIGCONT) < 0){
        fprintf(stderr, BRED "ERROR: Failed to continue background process with pid %d\n" CRESET, pid);
        return;
    }
    // printf("Background process with pid %d resumed successfully\n", pid);
    return;

}