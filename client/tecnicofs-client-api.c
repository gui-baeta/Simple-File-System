# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/un.h>

# include "tecnicofs-client-api.h"

# define YES 1
# define NO 0
# define NO_ERRORS 0
# define ERROR -1

# define MAX_INPUT_SIZE 2048

_Bool active_session = 0;

struct sockaddr_un serv_addr = {AF_UNIX, {0}};
int sockfd;
socklen_t serv_addr_sz;
char unixstream_path[108];
int ret_msg;

int sessionIsActive() {
    return active_session ? YES : NO;
}

int tfsCreate(char *filename, permission ownerPermissions, permission othersPermissions) {
    if (sessionIsActive() == NO) {
        return TECNICOFS_ERROR_NO_OPEN_SESSION;
    }
    char buffer[MAX_INPUT_SIZE];
    // Puts arguments given by client in buffer
    sprintf(buffer, "c %s %d%d", filename, ownerPermissions, othersPermissions);
    // Sends buffer to server
    if (write(sockfd, buffer, strlen(buffer) + 1) == ERROR) {
        perror("Couldn't write to server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    // Receives from server integer: 0 if the opperation given by command succeeds; error code if it doesn't
    if (read(sockfd, &ret_msg, 64) == ERROR) {
        perror("Couldn't read from server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    return ret_msg;
}

int tfsDelete(char *filename) {

    if (sessionIsActive() == NO) {
        return TECNICOFS_ERROR_NO_OPEN_SESSION;
    }
    char buffer[MAX_INPUT_SIZE];
    // Puts arguments given by client in buffer
    sprintf(buffer, "d %s", filename);
    // Sends buffer to server
    if (write(sockfd, buffer, strlen(buffer) + 1) == ERROR) {
        perror("Couldn't write to server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    // Receives from server integer: 0 if the opperation given by command succeeds; error code if it doesn't
    if (read(sockfd, &ret_msg, 64) == ERROR) {
        perror("Couldn't read from server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    return ret_msg;
}

int tfsRename(char *filenameOld, char *filenameNew) {

    if (sessionIsActive() == NO) {
        return TECNICOFS_ERROR_NO_OPEN_SESSION;
    }
    char buffer[MAX_INPUT_SIZE];
    // Puts arguments given by client in buffer
    sprintf(buffer, "r %s %s", filenameOld, filenameNew);
    // Sends buffer to server
    if (write(sockfd, buffer, strlen(buffer) + 1) == ERROR) {
        perror("Couldn't write to server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    // Receives from server integer: 0 if the opperation given by command succeeds; error code if it doesn't
    if (read(sockfd, &ret_msg, 64) == ERROR) {
        perror("Couldn't read from server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    return ret_msg;
}

int tfsOpen(char *filename, permission mode) {

    if (sessionIsActive() == NO) {
        return TECNICOFS_ERROR_NO_OPEN_SESSION;
    }
    char buffer[MAX_INPUT_SIZE];
    // Puts arguments given by client in buffer
    sprintf(buffer, "o %s %d", filename, mode);

    // Sends buffer to server
    if (write(sockfd, buffer, strlen(buffer) + 1) == ERROR) {
        perror("Couldn't write to server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    // Receives from server integer: 0 if the opperation given by command succeeds; error code if it doesn't
    if (read(sockfd, &ret_msg, 64) == ERROR) {
        perror("Couldn't read from server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    return ret_msg;
}

int tfsClose(int fd) {

    if (sessionIsActive() == NO) {
        return TECNICOFS_ERROR_NO_OPEN_SESSION;
    }
    char buffer[MAX_INPUT_SIZE];
    // Puts arguments given by client in buffer
    sprintf(buffer, "x %d", fd);

    // Sends buffer to server
    if (write(sockfd, buffer, strlen(buffer) + 1) == ERROR) {
        perror("Couldn't write to server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    // Receives from server integer: 0 if the opperation given by command succeeds; error code if it doesn't
    if (read(sockfd, &ret_msg, 64) == ERROR) {
        perror("Couldn't read from server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    return ret_msg;
}

int tfsRead(int fd, char *readBuffer, int len) {

    if (sessionIsActive() == NO) {
        return TECNICOFS_ERROR_NO_OPEN_SESSION;
    }
    char buffer[MAX_INPUT_SIZE];
    // Puts arguments given by client in buffer
    sprintf(buffer, "l %d %d", fd, len);

    // Sends buffer to server
    if (write(sockfd, buffer, strlen(buffer) + 1) == ERROR) {
        perror("Couldn't write to server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    // Receives from server file content read
    if (read(sockfd, readBuffer, len) == ERROR) {
        perror("Couldn't read from server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    // Receives from server integer: 0 if the opperation given by command succeeds; error code if it doesn't
    if (read(sockfd, &ret_msg, 64) == ERROR) {
        perror("Couldn't read from server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    return ret_msg;
}

int tfsWrite(int fd, char *dataInBuffer, int len) {

    if (sessionIsActive() == NO) {
        return TECNICOFS_ERROR_NO_OPEN_SESSION;
    }
    char buffer[MAX_INPUT_SIZE];

    char command[MAX_INPUT_SIZE];
    sprintf(command, "w %d", fd);
    snprintf(buffer, len + 1 + strlen(command) + 1, "%s %s", command, dataInBuffer);

    // Sends buffer to server
    if (write(sockfd, buffer, strlen(buffer) + 1) == ERROR) {
        perror("Couldn't write to server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    // Receives from server integer: 0 if the opperation given by command succeeds; error code if it doesn't
    if (read(sockfd, &ret_msg, 64) == ERROR) {
        perror("Couldn't read from server socket");
        return TECNICOFS_ERROR_OTHER;
    }
    return ret_msg;
}

int tfsMount(char * address) {
    if (sessionIsActive() == YES) {
        return TECNICOFS_ERROR_OPEN_SESSION;
    }
    strcpy(serv_addr.sun_path, address);
    // Creates file descriptor
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == ERROR) {
        perror("Couldn't open socket");
        return TECNICOFS_ERROR_OTHER;
    }

    serv_addr_sz = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
    // Establishes a connection with the server
    if (connect(sockfd, (struct sockaddr *) &serv_addr, serv_addr_sz) == ERROR) {
        perror("Couldn't connect to server");
        return TECNICOFS_ERROR_CONNECTION_ERROR;
    }
    active_session = YES;
    return NO_ERRORS;
}

int tfsUnmount() {
    if (sessionIsActive() == YES) {
        // Command to end the session
        write(sockfd, "e", 64);
        // Ends the connection with the server
        if (close(sockfd)) {
            perror("Couldn't unmount TecnicoFileSystem");
            return TECNICOFS_ERROR_CONNECTION_ERROR;
        }
        return NO_ERRORS;
    } else {
        return TECNICOFS_ERROR_NO_OPEN_SESSION;
    }
}
