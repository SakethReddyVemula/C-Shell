#ifndef __SEEK_H_
#define __SEEK_H_

// Struct to store flags information
typedef struct {
    bool d;
    bool f;
    bool e;
} SeekFlags;

// Struct to store information related to recent file which was matched
typedef struct {
    char path[MAX_PATH_LEN + 1];
    bool is_dir;
    bool has_read;
    bool has_exec;
} RecentMatch;


void print_error(const char* message);
void print_warning(const char* message);
void print_match(const char* path, bool is_dir);
bool parse_flags(char* token, SeekFlags* flags);
char* get_target_directory(char* token);
void update_recent_match(const char* path, bool is_dir, bool has_read, bool has_exec) ;
int seek(const char* target_file_name, const char* target_path, const SeekFlags* flags);
void handle_e_flag(const char* curr_directory_path);
void handleFlagsAndSeek(char* token);


#endif