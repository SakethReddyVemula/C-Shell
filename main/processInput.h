#ifndef __PROCESSINPUT_H
#define __PROCESSINPUT_H

int parseInput(char* command, char** inputCmnds);
void cleanString(char* str);
void replaceChars(char* str, char delimiter, int* end);
void handle_input_redirection(char* input_file_name);
void handle_output_redirection(char* output_file_name, int append);
int handlePipes(char* cmnd);
void runCmnd(char* cmnd);
void processQuotes(char* cmnd, char delimiter);
int checkInvalidPipes(char* cmnd);
// typedef struct command{
//     int idx;
//     char* token
// }

#endif