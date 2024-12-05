#include "../utils/headers.h"

/*  Handle case when <path/name> is home directory of C-shell   */
void handleHomeDirReveal(char* token, bool l, bool a){
    char finalPath[MAX_PATH_LEN] = {};
    strcat(finalPath, abs_path_home_dir);
    strcat(finalPath, token + 1); // ignore '~' and copy anything remaining
    reveal(finalPath, l, a);
}

/*  Handle different combinations of <flags> and <path/name> arguments  */
void handleFlagsAndReveal(char* token){
    bool l = false;
    bool a = false;
    
    // case: no <flags> or <path/name> given
    if(token == NULL){
        reveal(NULL, l, a);
        return;
    } else{
        /*  Handle all different possible combinations of l and a flags */
        while(token != NULL && token[0] == '-'){
            // found operator
            if(token[1] == '\0'){
                // found no flags
                fprintf(stderr, BRED "ERROR: command reveal: Found operator \"-\" but not flags such as \"l\" or \"a\"" CRESET);
                return;
            }
            for(int i = 1; token[i] != '\0'; i++){
                if(token[i] == 'a'){
                    a = true;
                } else if(token[i] == 'l'){
                    l = true;
                } else{
                    fprintf(stderr, BRED "ERROR: command reveal: Invalid flag for <flags>. Expected \"l\" or \"a\"" CRESET);
                    return;
                }
            }
            token = strtok(NULL, " \t\n"); // next
        }
        /*  Empty <path/name> => No directory given as argument  */
        if(token == NULL){
            reveal(NULL, l, a);
            return;
        } else{
            /*  Directory path <path/name> given as argument    */
            if(token[0] == '~'){
                /* Handle home directory path   */
                handleHomeDirReveal(token, l, a);
                return;
            } else{
                reveal(token, l, a);
            }
        }
    }
    
}


/**
 * @brief Prints the details of a file or directory.
 *
 * This function takes a directory entry, the parent directory path, and a flag
 * indicating whether to display long-format details. It then prints the file
 * or directory details in the appropriate format.
 *
 * @param entry The directory entry to print details for.
 * @param path The parent directory path.
 * @param long_format Flag indicating whether to display long-format details.
 */
void print_file_details(struct dirent* entry, char* path, bool l){
    struct stat file_stat; // for recieving information about the file.
    char req_dir[MAX_PATH_LEN * 2];
    char permissions[PERMISSION_STR_LEN];
    char time_str[MAX_TIME_STR_LEN];
    struct passwd* pw; // store the details of the owner of file.
    struct group* gr; // store the details of group of file.

    strcpy(req_dir, path);

    // get the file status information using lstat()
    if(lstat(req_dir, &file_stat) == -1){
        fprintf(stderr, BRED "ERROR: lstat inside print_file_details\n" CRESET);
        return;
    }

    // construct the file permissions string
    // DIR -> directory, BLK -> Block device, CHR -> character device, FIFO -> FIFO/pipe, LNK -> Symlink, SOCK -> Socket
    // Source: Linux file types: https://www.bogotobogo.com/Linux/linux_File_Types.php#google_vignette
    unsigned int fileType = file_stat.st_mode & __S_IFMT;
    if(fileType == __S_IFBLK){
        permissions[0] = 'b'; // Block Device
    } else if(fileType == __S_IFCHR){
        permissions[0] = 'c'; // Character Device
    } else if(fileType == __S_IFDIR){
        permissions[0] = 'd'; // Directory
    } else if(fileType == __S_IFIFO){
        permissions[0] = 'p'; // FIFO/pipe
    } else if(fileType == __S_IFLNK){
        permissions[0] = 'l'; // Symlink
    } else if(fileType == __S_IFSOCK){
        permissions[0] = 's'; // Socket
    } else{
        permissions[0] = '-'; // Regular file
    }
    permissions[1] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';

    // if -l flag set, print detailed file information
    if(l){
        // get the modification time and format it
        strftime(time_str, sizeof(time_str), "%b %d %H:%M", localtime(&file_stat.st_mtime));

        // get the user and group names
        pw = getpwuid(file_stat.st_uid);
        gr = getgrgid(file_stat.st_gid);

        // print the file details
        // %4ld -> pad by 4 units
        // %-15s -> Pad by 15 units and left align
        // %9zu -> Pad by 5 units
        printf(CRESET "%s %4ld %-15s %-15s %9zu %s" CRESET, permissions, file_stat.st_nlink, pw->pw_name, gr->gr_name, file_stat.st_size, time_str);
        if(fileType == __S_IFDIR){
            printf(BBLU " %s" CRESET, entry->d_name); // Blue for Directories
        } else if(fileType == __S_IFLNK){
            printf(BCYN " %s" CRESET, entry->d_name); // Cyan for Symlinks
        } else if(file_stat.st_mode & S_IXUSR){
            printf(BGRN " %s" CRESET, entry->d_name); // Green for executable files (have executable permission by owner)
        } else{
            printf(WHT " %s" CRESET, entry->d_name); // White for Regular files
        }
        // Display the path that they are linked to for symlink files
        char tempBuffer[MAX_PATH_LEN] = {};
        if(fileType == __S_IFLNK){
            readlink(req_dir, tempBuffer, MAX_PATH_LEN);
            printf(BCYN " -> %s" CRESET, tempBuffer);
        }
        printf("\n"); // newline at end
    } else{
        // if -l flag not set, print just file names with specific color
        unsigned int fileType = file_stat.st_mode & __S_IFMT; // determining file type
        if(fileType == __S_IFDIR){
            printf(BBLU "%s  " CRESET, entry->d_name); // Blue -> directories
        } else if(fileType == __S_IFLNK){
            printf(BCYN "%s  " CRESET, entry->d_name); // Cyan -> sumlinks
        } else if(file_stat.st_mode & S_IXUSR){
            printf(BGRN "%s  " CRESET, entry->d_name); // Green -> executable files which must have execute permissions in Owner
        } else{
            printf(WHT "%s  " CRESET, entry->d_name); // White -> files
        }
    }

}

int compare_entries(const struct dirent** a, const struct dirent** b){
    return strcmp((*a)->d_name, (*b)->d_name); // returns less, equal, greater wrt 0s
}

/**
 * @brief Reveals the contents of a directory.
 *
 * This function lists all the files and directories in the specified directory
 * in lexicographic order. It supports the -a and -l flags to display all files
 * (including hidden files) and long-format details, respectively.
 *
 * @param path The path to the directory to reveal.
 * @param l Flag indicating whether to display long-format details.
 * @param a Flag indicating whether to show all files, including hidden files.
 */
void reveal(char* path, bool l, bool a){
    // printf("%d %d\n", l, a); // use for debugging l and a values are correct
    struct dirent** entries;
    struct stat stats; // to retrieve information about a file
    char req_dir[MAX_PATH_LEN * 2];

    // check if path is NULL
    if(path == NULL){
        getcwd(req_dir, MAX_PATH_LEN);
    } else{
        strcpy(req_dir, path);
    }

    // Source: https://man7.org/linux/man-pages/man3/scandir.3.html
    int retVal = scandir(req_dir, &entries, NULL, alphasort); // return value: -1 if failed, n_entries if successful
    if(retVal == -1){
        fprintf(stderr, BRED "Failed to scan the directory %s\n" CRESET, req_dir);
        return;
    }
    if(l){
        // Source: https://askubuntu.com/questions/1252657/why-is-total-stat-block-size-of-a-directory-twice-of-ls-block-size-of-a-director
        long block_size = 0;
        for (int i = 0; i < retVal; i++) {
            // Skip hidden files if 'a' is not set
            if (!a && entries[i]->d_name[0] == '.') {
                continue;
            }
            char file_path[3 * MAX_PATH_LEN] = {0}; // Initialize to zero
            snprintf(file_path, 3 * MAX_PATH_LEN, "%s/%s", req_dir, entries[i]->d_name);
            lstat(file_path, &stats);
            block_size += stats.st_blocks;
        }
        printf(CRESET "total %ld\n" CRESET, block_size);
    }

    // iterate through the directory entries
    // for(i = 0; i < n_entries; i++){
    for(int i = 0; i < retVal; i++){
        if(entries[i]->d_name[0] == '.' && !a){
            continue;
        }
        char filePath[MAX_PATH_LEN * 2] = {};
        // snprintf(filePath, MAX_PATH_LEN, "%s/%s", req_dir, entries[i]->d_name); // Invalid
        strcat(filePath, req_dir);
        strcat(filePath, "/");
        strcat(filePath, entries[i]->d_name);
        print_file_details(entries[i], filePath, l);
    }
    if(!l){
        printf("\n"); // print new line if l is not set at end
    }

    // free the memory used by the entries array
    for (int i = 0; i < retVal; i++) {
        free(entries[i]);
    }
    free(entries);
}