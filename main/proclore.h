#ifndef __PROCLORE_H_
#define __PROCLORE_H_

void proclore(char* token);
int get_pid(char* token);
int read_process_info(int pid, char* status, int* pgrp, int* tpgid, unsigned long* vsize);
void check_foreground_status(char* status, int pgrp, int tpgid);
int read_executable_path(int pid, char* exec_path_input);


#endif