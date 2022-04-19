# ifndef DATA_H
# define DATA_H

# include "fs.h"
# include <time.h>
# include <semaphore.h>
# include <sys/socket.h>

# define MAX_INPUT_SIZE 2048
# define MAX_CLIENTS 50
# define MAX_FILE_CONTENT 2048
# define MAX_OPEN_FILES 5

//------------------------------------------------------------------------------
// Global data
//------------------------------------------------------------------------------
extern int numberBuckets; // Number of buckets
extern int numberLocks; // Number of locks
extern tecnicofs* fs; // tecnicoFyleSystem pointer

extern pthread_t thread_id[MAX_CLIENTS];

extern int client; // Number of active clients
extern int server_sockfd;
extern struct sockaddr_un server_addr;
extern socklen_t server_addr_sz;

extern struct timespec start, end;

typedef struct Open_files {
    int openINumber;
    int mode;
}Open_files;

# endif // DATA_H
