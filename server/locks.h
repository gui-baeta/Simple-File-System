# ifndef LOCKS_H
# define LOCKS_H

# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <pthread.h>
# include "data.h"
# include "macros.h"

int return_0(void *x);

void lockInit(Lock* lock_t, Lock_attr* attr);

void lockDeclare();

void lockDestroy();

void freeLocks();

void lock(size_t hash_key);

void lockR(size_t hash_key);

void unlock(size_t hash_key);

# endif // LOCKS_H
