#ifndef __PROMPT_H
#define __PROMPT_H

void getUserName(char* userName);
void getHostName(char* systemName);
void getCurrPath(char* currPath);
void getRelPath(char* relPath, char* currAbsPath, char* home_dir);
void printPrompt(char* userName, char* systemName, char* relPath);
void prompt();


#endif