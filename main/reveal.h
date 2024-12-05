#ifndef __REVEAL_H_
#define __REVEAL_H_

void handleHomeDirReveal(char* token, bool l, bool a);
void handleFlagsAndReveal(char* token);
void print_file_details(struct dirent* entry, char* path, bool l);
int compare_entries(const struct dirent** a, const struct dirent** b);
void reveal(char* path, bool l, bool a);

#endif