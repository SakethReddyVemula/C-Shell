#ifndef __HEADERS_H
#define __HEADERS_H

// Allowed headers: https://en.cppreference.com/w/c/header
// system Headers
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h> 
#include <sys/types.h> // defines various data types used in the system, such as `move_t`, `ino_t`, `dev_t`. These types are used in `struct stat` which is used to get information about files and directories.
#include <dirent.h> // defines the `DIR` and `struct dirent` types, which are used to iterate through the contents of a directory
#include <sys/stat.h> // defines macros and data structures for file status information, such as file modes, permissions, and timestamps. It is used to get information about files and directories.
#include <pwd.h> // provides functions for retrieval information about user accounts, such as `getpwuid()`. This information is used to display the file owner's username.
#include <grp.h> // provides functions for retriving information about group accounts, such as `getgrgid()`. This information is used to display the file group name.
#include <time.h>
#include <fcntl.h> // for O_CREAT in open() system call
#include <errno.h> // Used for handling errors after executing a system command; [sysCmnds.c]
#include <signal.h> // For handling signals (in bg, fg, ...), for SPEC: 14 Signals
#include <wait.h> // for using waitpid() system call
#include <ctype.h> // isspace() -> check for whitespace characters
// header files for network programming in C
#include <sys/socket.h> // provides functionalities such as socket() -> creating a new socket, connect()->establishing a connection to a server, send() and recv(), close()->closing a socket
#include <netinet/in.h> // provides definitions for the Internet Protocol family. Includes struct sockaddr_in. Also used when more generic getaddrinfo() is used
#include <arpa/inet.h> // provides functionalities for manipulating numeric IP addresses. 
// inet_addr() -> converting IP addresses from text to binary form (taken care by getaddrinfo())
// inet_ntoa() -> converting binary IP address to text form (taken care by getaddrinfo())
#include <netdb.h> // provides `struct addrinfo` used in `hints` and `res` variables
// provides `getaddrinfo()` -> resolving hostnames to an IP address, provides `freeaddrinfo()`

// new custom headers
#include "settings.h"
#include "../main/prompt.h"
#include "colors.h"
#include "../main/processInput.h"
#include "../main/hop.h"
#include "../main/reveal.h"
#include "../main/log.h"
#include "../main/proclore.h"
#include "../main/seek.h"
#include "../main/sysCmnds.h"
#include "../main/bgHandler.h"
#include "../main/fgHandler.h"
#include "../main/myshrc.h"
#include "../main/signals.h"
#include "../networking/iMan.h"

// How to make visibility of a variables in C global?
// Sources: https://www.geeksforgeeks.org/understanding-extern-keyword-in-c/
// int var; -> declaration and definition
// extern var; -> declaration (no memory is allocated, compiler is only informed that a variable exists)
// Source: Difference between Declaration and Definition: https://stackoverflow.com/questions/1410563/what-is-the-difference-between-a-definition-and-a-declaration
extern char abs_path_home_dir[MAX_LEN]; // Only declaration
extern bool isForegroundProcess; // for handling fg and bg processes globally
extern int count_bg_processes; // Make it global (Used in Ctrl+D signal)
extern ProcessInfo* bg_processes[MAX_BG_PROCESSES];
extern bool shouldExit;

#endif