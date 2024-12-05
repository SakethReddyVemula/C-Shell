#include "../utils/headers.h"

// Signals: https://man7.org/linux/man-pages/man7/signal.7.html
// kill system call: https://man7.org/linux/man-pages/man2/kill.2.html
// Differences between ctrl+C and ctrl+Z: https://prateeksrivastav598.medium.com/understanding-ctrl-z-and-ctrl-c-in-the-linux-terminal-c28bd1e00dbe#:~:text=Ctrl%20%2B%20C%2C%20on%20the%20other,exit%20a%20process%20or%20command.
/*s
    Ctrl+C:
    - forcefully terminate a running process
    - terminal sends an interrupt signal (SIGINT) to the process, causing it to stop immediately
    - quick way to exit a process or command

    Ctrl+Z:
    - suspend a running process in the terminal
    - temporarily pause the execution of a program and move it to the background
    - allowing you to continue using the terminal

    Ctrl+D:
    - Log out of the shell (after killing all the processes)
    - should have no affect on the original terminal
*/

void ctrl_C(){ // Forcefuly terminate a running process
    // printf("\nCtrl+C interrupt\n");
    if(isForegroundProcess == false){
        printf(BYEL "\nNo current foreground processes are present\n" CRESET);
        // isForegroundProcess = true;
        // return;
    } else{
        isForegroundProcess = false; // set it to false
    }
    fflush(stdin); // clear all the content in stdin
    fflush(stdout); // clear all the content in stdout
}

// exit from the shell
void ctrl_D(){
    int temp_count_bg_process = count_bg_processes;
    for(int i = 0; i < count_bg_processes; i++){
        if(kill(bg_processes[i]->PID, SIGKILL) == -1){
            fprintf(stderr, BRED "ERROR: Failed to send SIGKILL to process with process_id %d\n" CRESET, bg_processes[i]->PID);
            // isForegroundProcess = true;
            return;
        } else{
            printf("Process with PID %d killed using SIGKILL\n", bg_processes[i]->PID);
            temp_count_bg_process--;
            free(bg_processes[i]->cmnd_name);
            free(&bg_processes[i]->PID);
            bg_processes[i] = NULL; // set it to NULL for further use later
        }
    }
    // set count_bg_processes to new actual bg processes
    count_bg_processes = temp_count_bg_process;
    fflush(stdin);
    shouldExit = true;
}

void ctrl_Z(){ // Usage: Suspending a Process (running process)
    // printf("\nCtrl+Z interrupt\n");
    if(isForegroundProcess == false){
        printf(BYEL "\nNo current foreground processes are present\n" CRESET);
        // isForegroundProcess = true;
        // return;
    } else{
        isForegroundProcess = false; // set it to false
    }
    fflush(stdin); // clear all the content in stdin
    fflush(stdout); // clear all the content in stdout
}

void signals(char* token){
    if(token == NULL){
        fprintf(stderr, BRED "ERROR: No arguments passed for ping.\nUsage: ping <pid> <signal_number>\n" CRESET);
        return;
    }
    int process_id = atoi(token); // store pid of the process to which signal has to be passed
    if(process_id > __INT_MAX__){ // If pid is greater than INT_MAX its invalid
        fprintf(stderr, BRED "ERROR: No such process found\n" CRESET);
        return;
    }
    if(check_proc_in_bg_proceses(process_id) == false){
        fprintf(stderr, BRED "ERROR: No such process found\n" CRESET);
        return;
    }
    
    token = strtok(NULL, " \t\n");
    if(token == NULL){
        fprintf(stderr, BRED "ERROR: argument <signal_number> is missing\nUsage: ping <pid> <signal_number>\n" CRESET);
        return;
    }
    int signal_number = atoi(token); // store the signal number which has to be passed
    signal_number = signal_number % 32;
    if(kill(process_id, signal_number) == -1){
        fprintf(stderr, BRED "ERROR: Failed to send signal with signal number %d to process with pid %d\n" CRESET, signal_number, process_id);
        return;
    } else{
        printf("Sent signal %d to process with pid %d\n", signal_number, process_id);
        return;
    }
}