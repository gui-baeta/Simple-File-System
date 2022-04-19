# ifndef ENTRY_H
# define ENTRY_H

# include <string.h>
# include "data.h"
# include "fs.h"
# include "macros.h"
# include "locks.h"
# include "error.h"
# include "lib/hash.h"

# define FILE_NOT_OPEN -1

void createEntry(char* name, size_t hash_key, int iNumber);

int lookupEntry(char* name, size_t hash_key);

void deleteEntry(char* name, size_t hash_key);

int renameEntry(char* name_source, char* name_target, size_t hash_key_source);

_Bool isFileOpen(int token, Open_files open_files[5], char* option);

int createFile(char* str_a, char* str_b, size_t hash_key_a, uid_t uid);

int deleteFile(char* str_a, int hash_key_a, Open_files open_files[5], uid_t uid);

int renameFile(char* str_a, char* str_b, size_t hash_key_a, uid_t uid);

int openFile(char *filename, int mode, uid_t uid, size_t hash_key, Open_files open_files[5], int files_open);

int closeFile(int iNumber, Open_files open_files[5], int files_open);

int readFile(char* str_a, char* str_b, Open_files open_files[5], char buffer[MAX_FILE_CONTENT], size_t hash_key_a, uid_t uid);

int writeFile(char* str_a, char* str_b, Open_files open_files[5]);

void sendInt(int client_socket_fd, int msg);

void sendStr(int client_socket_fd, char* msg, int len);

# endif // ENTRY_H
