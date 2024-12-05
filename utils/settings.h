#ifndef __SETTINGS_H
#define __SETTINGS_H

#define MAX_LEN 4096
#define MAX_USERNAME_LEN 256 // LOGIN_NAME_MAX
#define MAX_SYSTEMNAME_LEN 64 // HOST_NAME_MAX
#define MAX_PATH_LEN 4096 // 
#define MAX_INPUT_LEN 4096 // <prompt> (command)
#define MAX_CMNDS 64 // max no.of number of commands in an input
#define MAX_CMND_LEN 256 // Individual command length
#define MAX_NUM_PIPES 128 // Not fixed -> Source: https://stackoverflow.com/questions/26322365/max-pipe-commands-of-linux
#define MAX_TIME_STR_LEN 100 // Used in reveal
#define PERMISSION_STR_LEN 11 // length of permission string for a file
#define MAX_CMNDS_IN_LOG 15 // Specification
#define LOG_FILE ".log" // Hidden file in shell directory for storing logs
#define MAX_ARGS_IN_A_CMND 256 // Maximum number of arguments in a single command (NOTE: command here is not input. Its parsed single command)
#define MAX_BG_PROCESSES 4096 // Maximum number of Background processes that we are assuming will run concurrently in single terminal shell instance.
#define MYSHRC_FILE ".myshrc" // Hidden file in shell directory for storing aliases and functions (Spec 9)
#define MAX_BUFFER_SIZE 1024 * 1024  // 1MB
#define HTTP_PORT "80"
#define MAN_HOST "man.he.net"

#endif