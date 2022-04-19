# include "locks.h"

//------------------------------------------------------------------------------
// Read/write lock functions
//------------------------------------------------------------------------------

int return_0(void *x) {
    return 0;
}

void lockInit(Lock* lock_t, Lock_attr* attr) {
    int err = defaultLock_init(lock_t, attr); // Initiate mutex lock...
    if (err) {                          // ...and check for errors
        perror("couldn't initialize mutex lock");
        exit(EXIT_FAILURE);
    }
}

void lockDeclare() {
    lock_t = malloc(sizeof(Lock)*numberLocks);

    for (size_t i = 0; i < numberLocks; i++) {
        lockInit(&lock_t[i], NULL);
    }
}

void lockDestroy() {
    for (size_t i = 0; i < numberLocks; i++) {
        int err = defaultLock_destroy(&lock_t[i]); // Deletes lock and checks...
        if (err) {                                 //...for errors
            perror("couldn't destroy mutex lock");
            exit(EXIT_FAILURE);
        }
    }
}

void freeLocks() {
    free(lock_t);
}

void lock(size_t hash_key) {
    int err = defaultLock_writelock(&lock_t[hash_key]);// Locks other threads...
    if (err) {                              // ... from writing on this resource
        perror("couldn't lock mutex lock");
        exit(EXIT_FAILURE);
    }
}

void lockR(size_t hash_key) {
    int err = defaultLock_readlock(&lock_t[hash_key]); // Locks other threads...
    if (err) {                                  // ...from reading this resource
        perror("couldn't lock read lock");
        exit(EXIT_FAILURE);
    }
}

void unlock(size_t hash_key) {
    int err = defaultLock_unlock(&lock_t[hash_key]);
    if (err) {
        perror("couldn't unlock mutex lock");
        exit(EXIT_FAILURE);
    }
}
