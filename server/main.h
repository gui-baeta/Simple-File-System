# ifndef MAIN_H
# define MAIN_H

# include <stdio.h>
# include <stdlib.h>
# include <getopt.h>
# include <string.h>
# include <ctype.h>
# include <time.h> // This library let us implement a timer
# include <pthread.h> // This library let us implement threads
# include <sys/un.h>
# include <unistd.h>
# include <signal.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include "data.h"
# include "locks.h"
# include "querieHandler.h"
# include "fs.h"
# include "lib/bst.h"
# include "lib/hash.h"
# include "error.h"

void displayError(char *err_m);

void displayUsage (const char* appName);

void verifyNumBuckets(char* numBuckets);

void parseArgs (long argc, char* const argv[]);

void receiveCommand(int client_socket_fd, char* buffer);

FILE* fpOpen(FILE* fp, char* input_file, char* options);

void fpClose(FILE* fp);

void *clientHandler(void* arg);

void timer(int option);

void createSocket();

void connectionsHandler();

void joinThreads();

# endif // MAIN_H
