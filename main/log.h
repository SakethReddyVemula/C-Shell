#ifndef __LOG_H_
#define __LOG_H_

typedef struct{
    char** commands;
    int count;
} Log;

void init_log(Log* log);
void free_log(Log* log);
char* get_log_path();
void load_log(Log* log);
void save_log(Log* log);
void handleLog(char* token);
bool is_valid_input(char* input);
void add_to_log(char* input);


#endif