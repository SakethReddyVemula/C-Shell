#include "../utils/headers.h"

// Set all flags to false initiallys
RecentMatch recent_match = {{0}, false, false, false};

/**
 * @brief Function to print error message into stderr
 * @param message Message to be print
 */
void print_error(const char* message){
    fprintf(stderr, BRED "ERROR: %s\n" CRESET, message);
}

/**
 * @brief Function to print a warning
 * @param message Message to be print
 */
void print_warning(const char* message){
    printf(YEL "%s\n" CRESET, message);
}

/**
 * @brief Function to print match result
 * 
 * Print BBLU if a directory, otherwise print BGRN
 * @param path Path of the matched file/folder
 * @param is_dir Boolean of whether the path is a directory or not
 */
void print_match(const char* path, bool is_dir){
    if(is_dir == true){
        printf(BBLU "%s\n" CRESET, path);
    } else{
        printf(BGRN "%s\n" CRESET, path);
    }
}

/**
 * @brief Handle different combinations of flags and store the info in SeekFlags struct
 * @param token String containing flags
 * @param flags Struct SeekFlags passed by reference
 */
bool parse_flags(char* token, SeekFlags* flags){
    // Found no flags
    if(token == NULL || token[0] != '-' || token[1] == '\0'){
        print_error("ERROR: command seek: Found operator \"-\" but not flags such as \"d\", \"f\" or \"e\"");
        return false;
    }


    for (int i = 1; token[i] != '\0'; i++){
        switch (token[i]){
            case 'd': flags->d = true; break;
            case 'f': flags->f = true; break;
            case 'e': flags->e = true; break;
            default:
                print_error("ERROR: command seek: Invalid flag for <flags>. Expected one of d/f/e");
                return false;
        }
    }

    // Both cannot be set at once
    if(flags->d == true && flags->f == true){
        print_error("Both 'd' and 'f' flags set simultaneously. Only one is allowed.");
        return false;
    }

    return true;
}

/**
 * @brief Update the information related to last matched file/directory
 * @param path Path to last matched file/directory
 * @param is_dir a directory?
 * @param has_read read permissions?
 * @param has_exec execute permissions?
 */
void update_recent_match(const char* path, bool is_dir, bool has_read, bool has_exec){
    strncpy(recent_match.path, path, MAX_PATH_LEN + 1);
    recent_match.path[MAX_PATH_LEN + 1] = '\0';
    recent_match.is_dir = is_dir;
    recent_match.has_read = has_read;
    recent_match.has_exec = has_exec;
}

/**
 * @brief Main function to seek a target file name in a specified target path
 * @param target_file_name <search>
 * @param target_path Path to directory inside which we seek
 * @param flags Struct of Flags (d, f, e)
 */
int seek(const char* target_file_name, const char* target_path, const SeekFlags* flags){
    // Track n_matches found
    int n_matches_found = 0;
    // printf("d: %d f: %d e: %d\n", d, f, e); // debugging helper 
    // store entries
    struct dirent** entries;
    struct stat stats; // to retrieve information about a file

    // Source: https://man7.org/linux/man-pages/man3/scandir.3.html
    int retVal = scandir(target_path, &entries, NULL, alphasort);
    if(retVal == -1){
        print_error("ERROR: Failed to scan <target_directory> in seek.");
        return 0;
    }

    // iterate through all the contents of the <target_directory>
    for (int i = 0; i < retVal; i++){
        // printf("Searching %s\n", entries[i]->d_name);
        // avoiding looping into same directories infinitely
        if(strcmp(entries[i]->d_name, ".") == 0 || strcmp(entries[i]->d_name, "..") == 0){
            continue;
        }

        char rel_file_path[MAX_PATH_LEN * 2] = {'\0'}; // Initialize with NULL
        strcat(rel_file_path, target_path);
        strcat(rel_file_path, "/");
        strcat(rel_file_path, entries[i]->d_name);

        if(lstat(rel_file_path, &stats) == -1){ // store info about file here.
            print_warning("Failed to get file stats.");
            continue;
        }

        // Info about file that is being searched
        bool is_dir = S_ISDIR(stats.st_mode);
        bool has_read = stats.st_mode & S_IRUSR;
        bool has_exec = stats.st_mode & S_IXUSR;

        if(is_dir == true && has_read == true && has_exec == true){
            // Recursive step to look inside another folder in current <target_directory>
            n_matches_found += seek(target_file_name, rel_file_path, flags);
        } else if(is_dir && !has_read){
            // print warning
            printf(BYEL "No read permissions for the directory \"%s\". Skiping...\n" CRESET, rel_file_path);
        } else if(is_dir && !has_exec){
            // print warning
            printf(BRED "No execute permissions for the directory \"%s\". Files with prefix \"%s\" in this directory will be searched, but those in sub-directories won't\n" CRESET, rel_file_path, target_file_name);
        }

        int prefix_len = strlen(target_file_name);
        if(strncmp(entries[i]->d_name, target_file_name, prefix_len) == 0){
            if((is_dir == true && flags->f == false) || (is_dir == false && flags->d == false)){
                n_matches_found++;
                // Print path of the matched file
                print_match(rel_file_path, is_dir);
                // Update recent struct to newly matched file info
                update_recent_match(rel_file_path, is_dir, has_read, has_exec);
            }
        }
        // free entries[i]
        free(entries[i]);
    }
    // free entries (struct dirent)
    free(entries);
    // return number of matches found
    return n_matches_found;
}

/**
 * @brief Handle 'e' set condition seperately
 * @param directory_path Path of current working directory of shell after changing directory to target path
 */
void handle_e_flag(const char* curr_directory_path){
    if(recent_match.has_read == false && recent_match.has_exec == false){
        print_error("ERROR: Missing permissions for task!");
    } else if(recent_match.is_dir == true && recent_match.has_exec == false){
        print_error("ERROR: Missing permissions for task!");
    } else if(recent_match.is_dir){
        chdir(recent_match.path);
        // Perform any necessary operations in the directory
        chdir(curr_directory_path);
    } else {
        if(recent_match.is_dir == true){
            chdir(recent_match.path);
            // is_final_dir_changed = true; // Not necessary
        } else{
            // Print the output of the file
            FILE* filePtr = fopen(recent_match.path, "r");
            char c = fgetc(filePtr);
            while(c != EOF){
                printf("%c", c);
                c = fgetc(filePtr);
            }
            fclose(filePtr);
        }
    }
}

/**
 * @brief Driver function to handle flags and Seek command
 * @param token String containing <flags> <search> <target_directory>*(optional)
 */
void handleFlagsAndSeek(char* token){
    // Initialize seek flags to false
    SeekFlags flags = {false, false, false};

    char curr_directory_path[MAX_PATH_LEN + 1] = {};

    // Store current working directory path for later use
    if(getcwd(curr_directory_path, MAX_PATH_LEN + 1) == NULL){
        print_error("ERROR: Failed to retrieve current directory.");
        return;
    }


    while (token != NULL && token[0] == '-'){
        // Handle all possible combinations of flag settings
        if(parse_flags(token, &flags) == false){
            return;
        }
        token = strtok(NULL, " \t\n");
    }

    // No <search> is given (Necessary)
    if(token == NULL){
        print_error("<search> is necessary.");
        return;
    }

    char* target_file_name = token;
    
    token = strtok(NULL, " \t\n"); // token contain either NULL or <target_directory> path

    char process_path[MAX_PATH_LEN + 1] = {};
    if(token != NULL){
        // <target_directory> path provided
        // handle home_dir case seperately
        if(token[0] == '~'){
            strcat(process_path, abs_path_home_dir);
            strcat(process_path, token + 1);
        } else{
            strcat(process_path, token);
        }
        if(chdir(process_path) == -1){
            fprintf(stderr, BRED "ERROR: failed to change directory to <target_director> in seek\n" CRESET);
            return;
        }
        // changed directory
    }
    // printf("processPath: %s\n", process_path);

    char path[] = "."; // always "." since, we already changed our directory of shell to <target_directory> already
    int n_matches = seek(target_file_name, path, &flags);

    // bool is_final_dir_changed = false; // Not necessary
    if(n_matches == 0){
        printf(BYEL "No Matches Found!\n" CRESET);
    } else if(n_matches == 1 && flags.e == true){
        // Handle case of 'e' flag set
        handle_e_flag(curr_directory_path);
    }
    // Hop to original current working directory
    chdir(curr_directory_path);
}