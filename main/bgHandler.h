#ifndef __BGHANDLER_H_
#define __BGHANDLER_H_

// For each process, we need to store PID and command name
typedef struct {
    int PID;
    char* cmnd_name;
} ProcessInfo;

void sort_bg_processes();
int custom_cmp(const void* A, const void* B);
void add_to_bg_processes_list(char* cmnd_name, int child_pid);
bool check_proc_in_bg_proceses(int child_pid);
void remove_process(int child_pid);
int process_BGSIGCHLD();
void sort_bg_processes();
ProcessInfo* create_process_info(int pid, char* cmnd_name);
void free_process_info(ProcessInfo* proc);
void print_process_details(ProcessInfo* proc);
void activities();
void bg(char* token);

#endif