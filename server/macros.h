# ifndef MACROS_H
# define MACROS_H

# include "pthread.h"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

pthread_rwlock_t* lock_t; // Defines read/write lock variables
# define Lock pthread_rwlock_t
# define Lock_attr pthread_rwlockattr_t
# define defaultLock_init(LOCK, ATTR) pthread_rwlock_init(LOCK, ATTR)
# define defaultLock_destroy(LOCK) pthread_rwlock_destroy(LOCK)
# define defaultLock_writelock(LOCK) pthread_rwlock_wrlock(LOCK)
# define defaultLock_readlock(LOCK) pthread_rwlock_rdlock(LOCK)
# define defaultLock_unlock(LOCK) pthread_rwlock_unlock(LOCK)

# endif // MACROS_H
