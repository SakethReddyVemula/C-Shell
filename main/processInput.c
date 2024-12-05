#include "../utils/headers.h"

// replace all continous spaces with a single space and replace tab with single space
void cleanString(char* str){ // TC: O(N)
    int i = 0;
    int j = 0;
    int space_found = 0; // flag
    char quote = '\0'; // tracks if we are inside quotes

    // skip leading spaces or tabs
    while(str[i] == ' ' || str[i] == '\t'){
        i++;
    }

    while(str[i] != '\0'){
        if(quote == '\0' && (str[i] == '\"' || str[i] == '\'')){
            // found opening quote, enter quoted mode
            quote = str[i]; // store which quote: " or '
            str[j++] = str[i++];
            continue;
        }
        if(quote != '\0' && str[i] == quote){
            // found closing quote, exit quoted mode
            quote = '\0';
            str[j++] = str[i++];
            continue;
        }
        if(quote != '\0'){
            // inside quotes, copy characters as-is
            str[j++] = str[i++];
            continue;
        }

        if(str[i] == '\t'){
            str[i] = ' '; // replace '\t' with a space
        }
        if(str[i] == ' '){
            if(space_found == 0){
                str[j++] = str[i]; // copy the first space found
                space_found = 1;
            }
        } else{
            str[j++] = str[i];
            space_found = 0;
        }
        i++;
    }
    // make remaining null characters
    while(j < i){
        str[j++] = '\0';
    }
}

// parse a single string containing multiple commands based on ';' and '&' #SPEC2
int parseInput(char* input, char** inputCmnds){
    int len = strlen(input);
    int numCmnds = 0;
    int len_cmnd = 0;
    char quote = '\0'; // tracks if we are inside quotes

    for(int i = 0; i < len && input[i] != '\0'; i++){
        if(quote == '\0' && (input[i] == '\"' || input[i] == '\'')){
            // found opening quote, enter quoted mode
            quote = input[i];
            continue;
        }
        if(quote != '\0' && input[i] == quote){
            // found closing quote, exit quoted mode
            quote = '\0';
            continue;
        }
        if(quote != '\0'){
            // inside quotes, copy as-is. Ignore ; and &
            while(input[i] != '\0' && input[i] != quote){
                len_cmnd++;
                i++;
            }
            if(input[i] == '\0'){ // end of string
                strncpy(inputCmnds[numCmnds], input + i - len_cmnd, len_cmnd);
                // Handle the case of last cmnd a null string; Ignore the cmnd of null string
                if(len_cmnd > 1){
                    numCmnds++;
                }
            } else if(input[i] == quote){
                // get outside quote mode
                i--;
                len_cmnd++;
                continue;
            }
            len_cmnd = 0;

        } else{
            // outside quotes, consider ; and &
            while(input[i] != '&' && input[i] != ';' && input[i] != '\0' && input[i] != '\"' && input[i] != '\''){
                len_cmnd++;
                i++;
            }
            
            if(input[i] == ';'){ // fg process
                strncpy(inputCmnds[numCmnds], input + i - len_cmnd, len_cmnd);
                numCmnds++;
            } else if(input[i] == '&'){ // bg process
                strncpy(inputCmnds[numCmnds], input + i - len_cmnd, len_cmnd);
                inputCmnds[numCmnds][len_cmnd] = ' '; // ensure a single space before '&'
                inputCmnds[numCmnds][len_cmnd + 1] = '&'; // mark the end with '&'
                numCmnds++;
            } else if(input[i] == '\0'){ // end of string
                strncpy(inputCmnds[numCmnds], input + i - len_cmnd, len_cmnd);
                // Handle the case of last cmnd a null string; Ignore the cmnd of null string
                if(len_cmnd > 1){
                    numCmnds++;
                }
            } else if(input[i] == '\"' || input[i] == '\''){
                // get into quote mode
                i--;
                len_cmnd++;
                continue;
            }
            len_cmnd = 0;
        }
    }
    for(int i = 0; i < numCmnds; i++){
        cleanString(inputCmnds[i]);
    }
    return numCmnds;
}

// Encode specific characters inside substrings which are enclosed inside quotes with non-printable ASCII control characters. Helps in later having special meaning to them.
void replaceChars(char* str, char delimiter, int* end){
    int i = 0;
    while(str[i] != '\0' && str[i] != delimiter){
        switch(str[i]){
            case ' ':
                str[i] = '\a'; // ASCII Bell
                break;
            case '\t':
                str[i] = '\f'; // ASCII Form Feed
                break;
            case '<':
                str[i] = '\r'; // ASCII Carriage Return
                break;
            case '>':
                str[i] = '\v'; // ASCII Vertical Tab
                break;
            case '|':
                str[i] = '\b'; // ASCII Backspace
                break;
            default:
                break;
        }
        i++;
        *end += 1;
    }
}

// Process single and double quotes in command seperately
void processQuotes(char* cmnd, char delimiter){
    char* start = strstr(cmnd, &delimiter); // points to first occurance of delimiter
    while(start){
        int end = 1;
        replaceChars(start + 1, delimiter, &end);
        start = strstr(start + end + 1, &delimiter);
    }
}

// Handle invalid pipess
int checkInvalidPipes(char* cmnd){
    // pipe at the start without non-blank character before it
    // Source: strspn -> https://man7.org/linux/man-pages/man3/strcspn.3p.html
    char* firstNonSpace = cmnd + strspn(cmnd, " \t\n");
    if(*firstNonSpace == '|'){
        fprintf(stderr, BRED "ERROR: Pipe occurs at beggining which is invalid since there is nothing to pipe input from\n" CRESET);
        return 1;
    }

    // pipe at the end without non-blank character after it
    // strchr -> first occurence of a character in str; strrchr -> last occurence of a character in str;
    // Source: strchr, strrchr -> https://www.man7.org/linux/man-pages/man3/strchr.3.html
    char* lastpipe = strrchr(cmnd, '|');
    if(lastpipe && strspn(lastpipe + 1, " \t\n") == strlen(lastpipe + 1)){
        fprintf(stderr, BRED "ERROR: Pipe occurs at end which is invalid since there is nothing to output to\n" CRESET);
        return 1;
    }

    // Multiple pipes together eg: "|||..."
    char* multiplePipes = strstr(cmnd, "||");
    if(multiplePipes){
        fprintf(stderr, BRED "ERROR: Multiple pipes with no commands in between. eg: |||..." CRESET);
        return 1;
    }
    return 0;
}


void handle_input_redirection(char* input_file_name){
    // printf("hello from input redirection handler\n");
    // printf("Input File: %s\n", input_file_name);
    int input_fd = open(input_file_name, O_RDONLY);
    if(input_fd < 0){
        fprintf(stderr, BRED "ERROR: Unable to redirect input from \"%s\".\nEnsure input file name is present and have read permissions.\n" CRESET, input_file_name);
        return;
    }

    if(dup2(input_fd, STDIN_FILENO) < 0){
        fprintf(stderr, BRED "Failed to duplicate STDIN\n" CRESET);
        return;
    }
    close(input_fd);
}

void handle_output_redirection(char* output_file_name, int append){
    int flags = O_CREAT | O_WRONLY;
    if(append == 1){
        flags |= O_APPEND;
    } else{
        if(access(output_file_name, F_OK) != -1){
            remove(output_file_name);
        }
    }
    int output_fd = open(output_file_name, flags, 0644);
    if(output_fd < 0){
        fprintf(stderr, BRED "ERROR: Unable to redirect output to \"%s\"\nEnsure sufficient permissions present for output file and its parent file\n" CRESET, output_file_name);
        return;
    }
    if(dup2(output_fd, STDOUT_FILENO) < 0){
        fprintf(stderr, BRED "Failed to duplicate STDOUT" CRESET);
        return;
    }
    close(output_fd);
}

// handle valid pipes
// Sources: https://man7.org/linux/man-pages/man2/pipe.2.htmls
int handlePipes(char* cmnd){
    char** pipe_commands = (char**)calloc(sizeof(char*), MAX_NUM_PIPES);
    for(int i = 0; i < MAX_NUM_PIPES; i++){
        pipe_commands[i] = NULL;
    }
    int n_pipes = 0; // number of pipe commands
    char* tempPipeCmnd = strtok(cmnd, "|");
    while(tempPipeCmnd){
        pipe_commands[n_pipes++] = tempPipeCmnd;
        // printf("pipe_command[i]: %s\n", pipe_commands[n_pipes - 1]);
        tempPipeCmnd = strtok(NULL, "|");
    }
    // printf("n_pipes: %d\n", n_pipes);
    int pipefd[n_pipes][2]; // pipefd[][0] -> read end of the pipe; pipefd[][1] -> write end of the pipe
    for(int i = 0; i < n_pipes - 1; i++){
        int p = pipe(pipefd[i]);
        if(p != 0){
            // error
            fprintf(stderr, BRED "ERROR: pipe failed to open. Possible reasons: FD out-of-bounds\n" CRESET);
            return 0;
        } else{
            // printf("pipe command successful. Enjoy\n");
        }
    }
    for(int i = 0; i < n_pipes; i++){ // if n_pipes = 3: 0, 1, 2
        // printf("Handling pipe no. %d\n", i);
        // save STDIN and STDOUT for piping purpose explicitly
        int saveSTDINfd = dup(STDIN_FILENO);
        int saveSTDOUTfd = dup(STDOUT_FILENO);
        if(saveSTDINfd == -1){
            fprintf(stderr, BRED "failed to save STDIN\n" CRESET);
            return 0;
        }
        if(saveSTDOUTfd == -1){
            fprintf(stderr, BRED "failed to save STDOUT\n" CRESET);
            return 0;
        }

        // Make preliminary changes to FDs based on the number of the pipe (basically output of one is input to next)
        if(i == 0 && n_pipes > 1){
            // if first pipe
            // printf("first pipe\n");
            // duplicate write end to STDOUT. close STDOUT
            if(dup2(pipefd[i][1], STDOUT_FILENO) == -1){
                // error
                fprintf(stderr, BRED "Failed to duplicate STDOUT\n" CRESET);
                return 0;
            }
            close(pipefd[i][1]);
        } else if(i < n_pipes - 1){
            // printf("middle pipes\n");
            if(dup2(pipefd[i][1], STDOUT_FILENO) == -1){
                fprintf(stderr, BRED "Failed to duplicate STDOUT\n" CRESET);
                return 0;
            }
            if(dup2(pipefd[i - 1][0], STDIN_FILENO) == -1){
                fprintf(stderr, BRED "Failed to duplicate STDIN\n" CRESET);
                return 0;
            }
            close(pipefd[i - 1][0]); // Close the same file which are duplicated. ERRORed (silent one) here. Be careful.
            close(pipefd[i][1]);
        } else if(i == n_pipes - 1 && i != 0){
            // if last pipe
            // printf("last pipe\n");
            if(dup2(pipefd[i - 1][0], STDIN_FILENO) == -1){
                fprintf(stderr, BRED "Failed to duplicate STDIN\n" CRESET);
                return 0;
            }
            // close(pipefd[i][0]);
            close(pipefd[i - 1][0]); // note: i - 1 not i
        }


        // Handle piping and redirection
        char strippedCmnd[MAX_CMND_LEN] = {"\0"};
        char* token = NULL;
        
        int append = 0; // 1 if '>>' else 0
        int redirect_input = 0;
        int redirect_output = 0;

        char output_file_name[MAX_PATH_LEN] = {};
        char input_file_name[MAX_PATH_LEN] = {};
        char new_cmnd[MAX_PATH_LEN] = {};
        // printf("hello\n");


        // check for input redirection
        token = pipe_commands[i];
        char* find_input_redir = strstr(token, "<");
        if(find_input_redir != NULL){
            redirect_input = 1;
            for(int j = 1; find_input_redir[j] != '\0'; j++){
                if(!isspace(find_input_redir[j])){ // loop until we find first non empty character
                    for(int k = j; find_input_redir[k] != '\0' && find_input_redir[k] != ' ' && find_input_redir[k] != '>' && find_input_redir[k] != '\n'; k++){
                        input_file_name[k - j] = find_input_redir[k];
                    }
                    break;
                }
            }
        }
        // printf("input_file_name: %s\n", input_file_name);

        char* find_non_append_redir = strstr(token, ">");
        char* find_append_redir = strstr(token, ">>");
        
        if(find_append_redir || find_non_append_redir){
            redirect_output = 1;
            char* redirectPtr;
            int offset;

            if(find_append_redir){
                append = 1;
                redirectPtr = find_append_redir;
                offset = 2;
            } else{
                append = 0;
                redirectPtr = find_non_append_redir;
                offset = 1;
            }

            // Find the output file names
            for(int j = offset; *redirectPtr != '\0'; j++){
                if(!isspace(redirectPtr[j])){
                    int k;
                    for(k = 0; redirectPtr[j + k] != '\0' && !isspace(redirectPtr[j + k]) && redirectPtr[j + k] != '<'; k++){
                        output_file_name[k] = redirectPtr[j + k];
                    }
                    output_file_name[k] = '\0';
                    break;
                }
            }
        }

        // Handle the change of fd's for both input redirection and output redirection

        if(redirect_input == 1){
            handle_input_redirection(input_file_name);
            // printf("Handled input redirections\n");
        }
        if(redirect_output == 1){
            // printf("Handling output redirections\n");
            handle_output_redirection(output_file_name, append);
            
        }
        
        // Handle actual command execution

        // copy everything until first NULL or '<' or '>' character
        // char* token = pipe_commands[i];
        char storeStrippedCmnd[MAX_CMND_LEN + 1] = {};
        for(int i = 0; token[i] != '\0' && token[i] != '>' && token[i] != '<'; i++){
            strippedCmnd[i] = token[i];
            storeStrippedCmnd[i] = token[i];
        }

        token = strtok(strippedCmnd, " \t\n"); // get the command name

        if(strcmp(token, "hop") == 0){
            token = strtok(NULL, " \t\n"); // get the directory to which need to change
            hop(token);
        } else if(strcmp(token, "reveal") == 0){
            token = strtok(NULL, " \t\n");
            handleFlagsAndReveal(token);
        } else if(strcmp(token, "log") == 0){
            token = strtok(NULL, " \t\n");
            // printf("Handling log\n");
            handleLog(token);
        } else if(strcmp(token, "proclore") == 0){
            token = strtok(NULL, " \t\n");
            // printf("Handling proclore\n");
            proclore(token);
        } else if(strcmp(token, "seek") == 0){
            token = strtok(NULL, " \t\n");
            // printf("Handling Seek\n");
            handleFlagsAndSeek(token);
        } else if(strcmp(token, "activities") == 0){
            activities(); // Call activites function in `bgHandler.py`
        } else if(strcmp(token, "ping") == 0){
            token = strtok(NULL, " \t\n");
            signals(token); // Call signals function in signals.c
        } else if(strcmp(token, "exit") == 0 || strcmp(token, "logout") == 0){
            ctrl_D(); // logout of the shell
        } else if(strcmp(token, "iMan") == 0){
            token = strtok(NULL, " \t\n");
            iMan(token); // handle networking of iMan
        } else if(strcmp(token, "fg") == 0){
            token = strtok(NULL, " \t\n");
            fg(token); // handle fg functionality
        } else if(strcmp(token, "bg") == 0){
            token = strtok(NULL, " \t\n");
            bg(token); // handle bg functionality
        } else{
            int is_present_in_myshrc = handleMyShrc(token);
            if(is_present_in_myshrc == 1){
                continue;
            } else{
                // printf("making system call: %s\n", token);
                // System command or an Invalid command
                // handleSysCmnds(token);
                // printf("cmnd: %s\n", storeStrippedCmnd);
                char* check_background = strstr(storeStrippedCmnd, "&");
                if(check_background == NULL){
                    // printf("Foreground Process\n");
                } else{
                    // printf("Background Process\n");
                    *check_background = '\0'; // replace last '&' with null
                }
                
                bool is_background = check_background == NULL ? false : true;
                int retVal = make_system_call(storeStrippedCmnd, is_background);
                if(retVal == -1){
                    fprintf(stderr, BRED "failed to fork() inside sysCmnds.c\n" CRESET);
                }
            }
            
        }


        // restore STDIN and STDOUT fd used for piping
        if(dup2(saveSTDINfd, STDIN_FILENO) == -1){
            fprintf(stderr, BRED "Failed to restore STDIN fd (piping)" CRESET);
            return 0;
        }
        if(dup2(saveSTDOUTfd, STDOUT_FILENO) == -1){
            fprintf(stderr, BRED "Failed to restore STDOUT fd (piping)" CRESET);
            return 0;
        }
        close(saveSTDINfd);
        close(saveSTDOUTfd);
        // printf("Handled pipe no %d\n", i);
    }
    
    return 1;
}

void runCmnd(char* cmnd){
    isForegroundProcess = true;

    // save STDIN and STDOUT
    int saveSTDINfd_main = dup(STDIN_FILENO);
    int saveSTDOUTfd_main = dup(STDOUT_FILENO);
    if(saveSTDINfd_main == -1){
        perror("failed to save STDIN fd");
        return;
    }
    if(saveSTDOUTfd_main == -1){
        perror("failed to save STDOUT fd");
        return;
    }

    // Process Quotes
    processQuotes(cmnd, '\''); // process single quotes
    processQuotes(cmnd, '\"'); // process double quotes
    // printf("Processed String: %s\n", cmnd);

    /*
        Piping: Handling and execution
        Error Handling: invalid pipe cases('|' at start, at end, and multiple pipes continously)
    */
    if(checkInvalidPipes(cmnd) == 1){ // check invalid pipes (start and end)
        return;
    } else{
        // printf("Valid Pipes\n");
    }
    
    if(handlePipes(cmnd) == 0){
        // error in handling pipes
        // printf("error in handling pipes\n");
        return;
    } else{
        // printf("Pipes handled\n");
        // Restore main STDIN and STDOUT
        if(dup2(saveSTDINfd_main, STDIN_FILENO) < 0){
            fprintf(stderr, BRED "Failed to restore STDIN\n" CRESET);
            return;
        }
        if(dup2(saveSTDOUTfd_main, STDOUT_FILENO) < 0){
            fprintf(stderr, BRED "Failed to restore STDOUT\n" CRESET);
            return;
        }
        close(saveSTDINfd_main);
        close(saveSTDOUTfd_main);

    }
}

