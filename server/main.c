//------------------------------------------------------------------------------
// Group: 071
// Members: Sofia Rodrigues - 92558 ; Guilherme Fontes - 92470
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header files, Libraries and Defined constants
//------------------------------------------------------------------------------

# define _GNU_SOURCE
# define __USE_GNU

# include "main.h"

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

int numberBuckets = 0; // Number of buckets
int numberLocks = 0; // Number of locks
tecnicofs* fs; // tecnicoFyleSystem pointer

pthread_t thread_id[MAX_CLIENTS];

struct timespec start, end; // Timer data

// Socket related variables
int client = 0; // Number of active clients
_Bool loop = 1;
int server_sockfd;
struct sockaddr_un server_addr = {AF_UNIX, {0}};
char unixstream_path[108];
socklen_t server_addr_sz;

//------------------------------------------------------------------------------
// Miscellaneous functions
//------------------------------------------------------------------------------

_Bool isNumeric(const char *str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (isdigit(str[i]) == NO)
            return NO;
    }
    return YES;
}

void displayError(char *err_m) { // Prints error messages
    fprintf(stdout, "Error: %s\n", err_m);
}

void displayUsage (const char* appName) { // Displays an usage error
    printf("Usage: %s\n", appName);
    exit(EXIT_FAILURE);
}

void verifyNumBuckets(char* str) {
    if (isNumeric(str)) {
        if (atoi(str) != 1){
            return;
        }
    }
    displayError("numbuckets must be a number greater than or equal to 1.");
    exit(EXIT_FAILURE);
}

void parseArgs (long argc, char* const argv[]) { // Checks if the...

    switch (argc) { // ...number of given arguments is right or wrong
        case 4:

            verifyNumBuckets(argv[3]);

            numberBuckets = atoi(argv[3]); // number of buckets
            numberLocks = numberBuckets;

            strcpy(unixstream_path, argv[1]); // Setting socket name

            break;
        default: // If less than 4 arguments were given, display an...
            puts("Invalid format:"); // ...error message
            displayUsage(argv[0]);
    }
}

FILE* fpOpen(FILE* fp, char* input_file, char* options) {
    fp = fopen(input_file, options); // Open given file if there isn't an error
    if (!fp) {
        sysCallError("Couldn't open file");
    }
    return fp;
}

void fpClose(FILE* fp) {
    if (fclose(fp)) { // Close given file if there isn't an error
        sysCallError("Couldnt't close file");
    }
}

//------------------------------------------------------------------------------
// Primary functions
//------------------------------------------------------------------------------

void *clientHandler(void* arg) { // Process commands...

    // Sets sigmask to force interrupt signal to be treated by main thread v
    sigset_t set;
    if (sigemptyset(&set) == ERROR){
        sysCallError("Couldn't empty set");
    }
    if (sigaddset(&set, SIGINT) == ERROR){
        sysCallError("Couldn't add SIGINT to set");
    }
    if (pthread_sigmask(SIG_BLOCK , &set, NULL) == ERROR){
        sysCallError("couldn't block sigmask");
    }
    // ^^^                           ^^^                                 ^^^

    int ret_value;

    char token;
    char str_a[MAX_INPUT_SIZE];
    char str_b[MAX_INPUT_SIZE];
    char buffer[MAX_FILE_CONTENT];
    char command[MAX_FILE_CONTENT];

    Open_files open_files[5];

    for (size_t i = 0; i < 5; i++) {
        open_files[i].mode = 0;
        open_files[i].openINumber = -1;
    }

    int files_open = 0;

    int client_socket_fd = *(int*) arg;

    struct ucred user_cred;

    socklen_t client_addr_sz = sizeof(server_addr);

    getsockopt(client_socket_fd, SOL_SOCKET, SO_PEERCRED, &user_cred,
               &client_addr_sz);

    uid_t uid = user_cred.uid; // Get the user ID out of the ucred struct

    while(1) {

        read(client_socket_fd, command, MAX_FILE_CONTENT);

        sscanf(command, "%c %s %s", &token, str_a, str_b);

        size_t hash_key_a = hash(str_a, numberBuckets);

        switch (token) {
            case 'c': // Create file

                ret_value = createFile(str_a, str_b, hash_key_a, uid);
                break;

            case 'd': // Delete file
                ret_value = deleteFile(str_a, hash_key_a, open_files, uid);
                break;

            case 'r': // Rename file
                ret_value = renameFile(str_a, str_b, hash_key_a, uid);
                break;

            case 'o': // Open file
                ret_value = openFile(str_a, atoi(str_b), uid, hash_key_a,
                                     open_files, files_open);
                break;

            case 'x': // Close file
                ret_value = closeFile(atoi(str_a), open_files, files_open);
                break;

            case 'l': // Read file
                ret_value = readFile(str_a, str_b, open_files, buffer,
                                     hash_key_a, uid);
                sendStr(client_socket_fd, buffer, atoi(str_b));
                break;

            case 'w': // Write to file
                ret_value = writeFile(str_a, str_b, open_files);
                break;

            case 'e':
                return NULL;
                break;

            default: // Tell the user that he didn't input any command
                displayError("Command not implemented");
                exit(EXIT_FAILURE);
        }
        // Sends integer to client
        sendInt(client_socket_fd, ret_value);
    }
}

//------------------------------------------------------------------------------
// Timer, Socket related operations, connections handler and Main function
//------------------------------------------------------------------------------

void timer(int option) {
    double time_span;
    int err;

    switch (option) {
        case 0: // Starting timer
            err = clock_gettime(CLOCK_REALTIME, &start);
            if (err) {
                sysCallError("couldn't start timer");
            }
            break;
        case 1: // Ending timer
            err = clock_gettime(CLOCK_REALTIME, &end);
            if (err) {
                sysCallError("couldn't end timer");
            }

            // Time calculations
            time_span = ( end.tv_sec - start.tv_sec )
            + ( end.tv_nsec - start.tv_nsec ) / 1000000000.0;
            printf("TecnicoFS completed in %0.4lf seconds.\n", time_span);
            break;
    }
}

//------------------------------------------------------------------------------
// Main thread runs this function after receiving interrupt signal...
void sigHandler(int sig) { // ...(CTRL + C) hotkey
    loop = NO; // Toggles accepting sessions loop off

    close(server_sockfd); // Closes the server socket so it doesn't receive...
                          // ...any more connections
    timer(1); // Ends timer
}

//------------------------------------------------------------------------------

void createSocket() { // Creates server side socket

    // Unlinks specified socket to start a fresh socket
    unlink(unixstream_path);

    // Copies specified socket path to socket address structure
    strcpy(server_addr.sun_path, unixstream_path);

    // Creates server side socket
    server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sockfd == ERROR) {
        sysCallError("Couldn't create socket file descriptor");
    }
    // Socket address's size
    server_addr_sz = strlen(server_addr.sun_path) + sizeof(server_addr.sun_family);
    // Binds socket name to socket
    if (bind(server_sockfd, (struct sockaddr *) &server_addr,
        server_addr_sz) == ERROR) {
        sysCallError("Couldn't bind unix address to the socket");
    }
}

//------------------------------------------------------------------------------

void connectionsHandler() { // Accepts client connections
    int temp_client_sockfd;

    if (listen(server_sockfd, 20) == ERROR) { // Listens for connections
        sysCallError("Error on system call listen");
    }

    timer(0);

    signal(SIGINT, sigHandler);

    while (loop) { // Accepts clients attempting to connect
        temp_client_sockfd = accept(server_sockfd, (struct sockaddr *) &server_addr,
            &server_addr_sz);
        if (loop == NO) { // If loop is toggled off
            return;
        }
        if (temp_client_sockfd == ERROR) {
            perror("Couldn't accept connection");
        }
        // Creates thread and sends client side socket to it
        if (pthread_create(&thread_id[client], NULL, &clientHandler, (void *)
            &temp_client_sockfd) == ERROR) {
            perror("Couldn't create thread");
        }

        client++;
        if (client == MAX_CLIENTS) { // Don't accept more client...
            loop = 0; // ...connections if reached max clients
        }
    }
}

void joinThreads() { // Wait for threads to end theirs jobs
    for (size_t i = 0; i < client; i++) {
        if (pthread_join(thread_id[i], NULL)) {
            sysCallError("Couldn't join thread");
        }
    }
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

    FILE *fp_out = NULL; // File pointer

    parseArgs(argc, argv); // Check given arguments

    lockDeclare();

    fp_out = fpOpen(fp_out, argv[2], "w");

    fs = newTecnicofs(argv);

    inode_table_init();

    createSocket(); // Create server side socket

    connectionsHandler(); // Accept client connections

    joinThreads(); // Wait for threads to end their jobs

    printTecnicofsTree(fp_out, fs); // Print the BST into the output...
                                      // ...text file
    freeLocks();

    freeTecnicofs(fs); // Free memory space occupied by tecnicoFS

    fpClose(fp_out); // Close output text file if there isn't an error

    exit(EXIT_SUCCESS); // Exit tecnicoFS program
}
