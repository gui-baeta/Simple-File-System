# include "error.h"

// Print system call error
void sysCallError(char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
