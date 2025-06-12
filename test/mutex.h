#ifndef MOCK_MUTEX_H
#define MOCK_MUTEX_H

// Mock mutex type for native environment
typedef struct {
    bool locked;
} mutex_t;

// Mock mutex functions
static inline void mutex_init(mutex_t* mutex) {
    mutex->locked = false;
}

static inline void mutex_lock(mutex_t* mutex) {
    mutex->locked = true;
}

static inline void mutex_unlock(mutex_t* mutex) {
    mutex->locked = false;
}

static inline bool mutex_trylock(mutex_t* mutex) {
    if (!mutex->locked) {
        mutex->locked = true;
        return true;
    }
    return false;
}

static inline void mutex_enter_blocking(mutex_t* mutex) {
    mutex_lock(mutex);
}

static inline void mutex_enter(mutex_t* mutex) {
    mutex_lock(mutex);
}

static inline void mutex_exit(mutex_t* mutex) {
    mutex_unlock(mutex);
}

#endif // MOCK_MUTEX_H