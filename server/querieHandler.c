# include "querieHandler.h"

//------------------------------------------------------------------------------
// Entry related functions
//------------------------------------------------------------------------------

void createEntry(char* name, size_t hash_key, int iNumber) {

    lock(hash_key); // Locks other threads from using this...
                    // ...bst. If read/write lock is defined...
                    // ..., threads are only locked to write
    create(fs, hash_key, name, iNumber); // Creates a directory entry
    unlock(hash_key); // Unlock directory lock protection
}

int lookupEntry(char* name, size_t hash_key) {
    lockR(hash_key); // Locks other threads from using this...
                     // ...bst. If read/write lock is defined...
                     // ..., threads are only locked to read
    int searchResult = lookup(fs, hash_key, name);
    unlock(hash_key);
    if (searchResult == ERROR) {
        printf("%s not found\n", name);
        return TECNICOFS_ERROR_FILE_NOT_FOUND;
    } else {
        printf("%s found with inumber %d\n", name, searchResult);
        return searchResult;
    }
}

void deleteEntry(char* name, size_t hash_key) {
    lock(hash_key); // Locks other threads from using this...
                    // ...bst. If read/write lock is defined...
                    // ...,threads are only locked to write

    delete(fs, hash_key, name);

    unlock(hash_key);
}

int renameEntry(char* name_source, char* name_target, size_t hash_key_source) {

    if (!strcmp(name_source, name_target)) {
        printf("Couldn't rename entry:\n");
        printf("    '%s' -> '%s'\n", name_source, name_target);
        printf("    * Source name '%s' equal to target name %s\n",
                name_source, name_target);
        return TECNICOFS_ERROR_OTHER;
    }
    size_t hash_key_target = hash(name_target, numberBuckets);

    _Bool same_bst = (hash_key_source == hash_key_target);

    lock(hash_key_source); // Locks source bst
    // Verifies if source name exists in system file
    if (same_bst == NO) {
        lock(hash_key_target); // Locks target bst
    }

    int iNumber_source = lookup(fs, hash_key_source, name_source);

    if (iNumber_source == ERROR) {
        printf("Couldn't rename entry:\n");
        printf("    '%s' -> '%s'\n", name_source, name_target);
        printf("    * Source name '%s' not found\n", name_source);

        unlock(hash_key_source); // Unlocks source bst
        if (same_bst == NO) {
            unlock(hash_key_target); // Unlock target bst
        }
        return TECNICOFS_ERROR_FILE_NOT_FOUND;
    }


    // Verifies if target name doesn't exist in system file
    int iNumber_target = lookup(fs, hash_key_target, name_target);

    if (iNumber_target != ERROR) {
        printf("Couldn't rename entry:\n");
        printf("    '%s' -> '%s'\n", name_source, name_target);
        printf("    * Target name '%s' already exists with inumber %d\n",
                name_target, iNumber_target);

        unlock(hash_key_source); // Unlocks source bst
        if (same_bst == NO) {
            unlock(hash_key_target); // Unlocks target bst
        }
        return TECNICOFS_ERROR_FILE_ALREADY_EXISTS;
    }

    delete(fs, hash_key_source, name_source); // Deletes file with source name

    create(fs, hash_key_target, name_target, iNumber_source); // Creates new file with target name
    unlock(hash_key_source);
    if (same_bst == NO) {
        unlock(hash_key_target); // Unlocks target bst
    }

    return NO_ERRORS;
}

// Verify if the client has this file already open
_Bool isFileOpen(int token, Open_files open_files[5], char* option) {
    // If fd (filedescriptor) option is triggered..., see if file is open
    if (!strcmp(option, "fd")) {
        return open_files[token].openINumber != FILE_NOT_OPEN;
    }
    // ...If it isn't, check if there is a open file with the specified iNumber
    for (size_t i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i].openINumber == token) {
            return YES;
        }
    }
    return NO;
}

int createFile(char* str_a, char* str_b, size_t hash_key_a, uid_t uid) {
    // Checks if new file name already exists
    if (lookupEntry(str_a, hash_key_a) != TECNICOFS_ERROR_FILE_NOT_FOUND) {
        return TECNICOFS_ERROR_FILE_ALREADY_EXISTS;
    }
    // Creates file inode with uid, owners permissions, others permissions and...
    // ...returns the newly created iNumber
    int iNumber = inode_create(uid, str_b[0] - '0', str_b[1] - '0');
    if (iNumber == ERROR) {
        return TECNICOFS_ERROR_OTHER;
    }
    // Creates file entry inside tfs BST
    createEntry(str_a, hash_key_a, iNumber);
    return NO_ERRORS;
}

int deleteFile(char* str_a, int hash_key_a, Open_files open_files[5], uid_t uid) {
    // Finds file's iNumber
    int iNumber = lookupEntry(str_a, hash_key_a);
    if (iNumber != TECNICOFS_ERROR_FILE_NOT_FOUND) {
        uid_t file_uid;
        // Gets file's inode
        if(inode_get(iNumber, &file_uid, NULL, NULL, NULL, 0) == ERROR) {
            return TECNICOFS_ERROR_OTHER;
        }
        // Checks if the user is the owner of the file to be delete
        if (uid != file_uid) {
            return TECNICOFS_ERROR_PERMISSION_DENIED;
        }
        if(inode_delete(iNumber) == ERROR) { // Deletes file
            return TECNICOFS_ERROR_OTHER;
        } else {
            deleteEntry(str_a, hash_key_a); // Deletes file from file entry BST
            return NO_ERRORS;
        }
    } else {
        return TECNICOFS_ERROR_FILE_NOT_FOUND;
    }
}

int renameFile(char* str_a, char* str_b, size_t hash_key_a, uid_t uid) {
    // Finds old file's name iNumber
    int iNumber_a = lookupEntry(str_a, hash_key_a);

    size_t hash_key_b = hash(str_b, numberBuckets);
    // Finds new file's name iNumber
int iNumber_b = lookupEntry(str_b, hash_key_b);

    if (iNumber_a == TECNICOFS_ERROR_FILE_NOT_FOUND) {
        return TECNICOFS_ERROR_FILE_NOT_FOUND;
    }
    if (iNumber_b != TECNICOFS_ERROR_FILE_NOT_FOUND) {
        return TECNICOFS_ERROR_FILE_ALREADY_EXISTS;
    }
    uid_t file_id;
    // Gets old file's name inode
    if(inode_get(iNumber_a, &file_id, NULL, NULL, NULL, 0) == ERROR) {
        return TECNICOFS_ERROR_OTHER;
    }
    if (uid != file_id) {
        return TECNICOFS_ERROR_PERMISSION_DENIED;
    } else {
        renameEntry(str_a, str_b, hash_key_a);
        return NO_ERRORS;
    }
}

int openFile(char *filename, int mode, uid_t uid, size_t hash_key, Open_files open_files[5], int files_open) {
    // Finds file's name iNumber
    int iNumber = lookupEntry(filename, hash_key);
    if (iNumber == TECNICOFS_ERROR_FILE_NOT_FOUND) {
        return TECNICOFS_ERROR_FILE_NOT_FOUND;
    }
    permission owner_permissions, others_permissions;
    uid_t file_id;
    if (inode_get(iNumber, &file_id, &owner_permissions, &others_permissions, NULL, 0) == ERROR) {
        return TECNICOFS_ERROR_OTHER;
    }
    // Checks if file's already open
    if (isFileOpen(iNumber, open_files, "iNumber") == YES) {
        return TECNICOFS_ERROR_FILE_IS_OPEN;
    }
    if (mode == 0) {
        return TECNICOFS_ERROR_INVALID_MODE;
    }
    if (((uid == file_id) && (mode & owner_permissions))
        ||
        ((uid != file_id) && (mode & others_permissions))) {
        if (files_open == 5) {
            return TECNICOFS_ERROR_MAXED_OPEN_FILES;
        }
        for (size_t i = 0; i < MAX_OPEN_FILES; i++) {
            if (open_files[i].openINumber == -1) {
                open_files[i].openINumber = iNumber;
                open_files[i].mode = mode;
                files_open++;
                return i;
            }
        }
        return TECNICOFS_ERROR_OTHER;
    } else {
        return TECNICOFS_ERROR_PERMISSION_DENIED;
    }
}

int closeFile(int iNumber, Open_files open_files[5], int files_open) {
    for (size_t i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i].openINumber == iNumber) {
            open_files[i].openINumber = -1;
            files_open--;
            return NO_ERRORS;
        }
    }
    return TECNICOFS_ERROR_FILE_NOT_OPEN;
}

int readFile(char* str_a, char* str_b, Open_files open_files[5], char buffer[MAX_FILE_CONTENT], size_t hash_key_a, uid_t uid) {
    int fd = atoi(str_a);
    int len = atoi(str_b);
    if (isFileOpen(fd, open_files, "fd") == YES) {
        permission owner_permissions, others_permissions;
        uid_t file_id;
        int cont_len = inode_get(open_files[fd].openINumber, &file_id, &owner_permissions,
            &others_permissions, buffer, len - 1);
        if (cont_len == ERROR) {
            return TECNICOFS_ERROR_OTHER;
        }
        if (READ & open_files[fd].mode) {
            return cont_len;
        } else {
            return TECNICOFS_ERROR_INVALID_MODE;
        }
    } else {
        return TECNICOFS_ERROR_FILE_NOT_OPEN;
    }
}

int writeFile(char* str_a, char* str_b, Open_files open_files[5]) {
    int fd = atoi(str_a);
    // Checks if file is open
    if (isFileOpen(fd, open_files, "fd") == NO) {
        return TECNICOFS_ERROR_FILE_NOT_OPEN;
    }

    int iNumber = open_files[fd].openINumber;
    int mode = open_files[fd].mode;
    // Verifies if file is opened in write or read/write mode
    if (!(mode & WRITE)) {
        return TECNICOFS_ERROR_PERMISSION_DENIED;
    }
    // Writes content given by str_b in file
    if (inode_set(iNumber, str_b, strlen(str_b)) == ERROR) {
        return TECNICOFS_ERROR_OTHER;
    }
    return NO_ERRORS;
}

// Sends integer to client
void sendInt(int client_socket_fd, int msg) {
    if (write(client_socket_fd, &msg, 64) == ERROR) {
        sysCallError("Couldn't write to client socket");
    }
}

// Sends String to client
void sendStr(int client_socket_fd, char* str, int len) {
    if (write(client_socket_fd, str, len) == ERROR) {
        sysCallError("Couldn't write to client socket");
    }
}
