#include "../utils/headers.h"

// Resources: 
// https://www.youtube.com/watch?v=3MZjaZxZYrE&list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY&index=17 - Stopping and continuing the execution of a program
// https://www.youtube.com/watch?v=7ud2iqu9szk&list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY&index=18 - Background and Foreground processes


int return_to_fg_method(char* cmdName, int childPID){
    add_to_bg_processes_list(cmdName, childPID); // Add process to background list.

    int status;
    signal(SIGTTIN, SIG_IGN); // ignore terminal read signals for the child.
    signal(SIGTTOU, SIG_IGN); // ignore terminal write signals for the child.
    
    tcsetpgrp(STDIN_FILENO, childPID); // give the child process control of the terminal.
    
    waitpid(childPID, &status, WUNTRACED); // wiit for the child process to change state.

    if (WIFEXITED(status) || WIFSIGNALED(status)) {
        remove_process(childPID); // remove process if it exited or was terminated.
    }

    tcsetpgrp(STDIN_FILENO, getpgid(0)); // return control of the terminal to the parent.

    signal(SIGTTIN, SIG_DFL); // restore default signal handling for terminal reads.
    signal(SIGTTOU, SIG_DFL); // restore default signal handling for terminal writes.

    if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE) {
        return -1; // command execution failed.
    } else if (WIFSTOPPED(status)) {
        return 0; // child process stopped (e.g., due to Ctrl-Z).
    }
    return 0;
}

// bring a running or stopped background with corresponding pid to foreground, handling it the control of terminal.
void fg(char* token){
    if(token == NULL){
        fprintf(stderr, BRED "ERROR: Correct Usage: fg <pid>\n" CRESET);
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
        fprintf(stderr, BRED "ERROR: Process with PID %d doesn't exist. No such background process found.\n" CRESET, pid);
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

    // store the status of child process determined by waitpid() in parent
    int child_status;

    // Ignore terminal I/O signals for the child process
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    // give terminal control to the child process
    tcsetpgrp(STDIN_FILENO, pid);

    // continue the process
    if(kill(pid, SIGCONT) < 0){
        fprintf(stderr, BRED "Failed to continue the process with pid %d\n" CRESET, pid);
        return;
    }

    // wait for the child process to finish or stop
    waitpid(pid, &child_status, WUNTRACED);

    // remove the process from the list unless it's moved to background
    if(WEXITSTATUS(child_status) != SIGTSTP){
        remove_process(pid);
    }

    // return terminal control to the parent process
    tcsetpgrp(STDIN_FILENO, getpgid(0));

    // restore default handling of terminal I/O signals
    signal(SIGTTIN, SIG_DFL); // restore default signal handling for terminal reads.
    signal(SIGTTOU, SIG_DFL); // restore default signal handling for terminal writes.

    // check if the child process failed to execute
    if (WEXITSTATUS(child_status) == EXIT_FAILURE) {
        fprintf(stderr, BRED "ERROR: Failed to bring process to foreground. Invalid PID %d.\n" CRESET, pid);
    }
    
}
