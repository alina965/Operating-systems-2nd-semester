#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/syscall.h>
#include <linux/unistd.h>

#include "my-thread.h"

typedef struct {
    void *(*start_routine)(void *);
    void *arg;
} thread_args_t;

int thread_function(void* arg) {
    thread_args_t *thread_args = (thread_args_t *)arg;
    void *(*start_routine)(void *) = thread_args->start_routine;
    void *routine_arg = thread_args->arg;
    
    free(thread_args);
    
    start_routine(routine_arg);
    
    return 0;
}

int my_pthread_create(mythread_t *thread, void *(start_routine), void *arg) {
    thread_args_t *thread_args = malloc(sizeof(thread_args_t));
    if (!thread_args) {
        return 1;
    }
    
    thread_args->start_routine = start_routine;
    thread_args->arg = arg;

    // создаем стек
    size_t stack_size = 1024 * 1024; // 1Mb
    void *stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (stack == MAP_FAILED) {
        free(thread_args);
        return 1;
    }

    int flags = CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_SYSVSEM|CLONE_THREAD;

    int result = clone(thread_function, (char*)stack + stack_size, flags, thread_args);
    if (result == -1) {
        munmap(stack, stack_size);
        free(thread_args);
        return 1;
    }

    if (thread)
        *thread = result;

    return 0;
}

// CLONE_VM the calling process and the child process run in the same memory space
// CLONE_FS the caller and the child process share the same filesystem information
// CLONE_FILES the calling process and the child process share the same file  descriptor  table
// CLONE_SIGHAND the calling process and the child process share the same table of signal handlers
// CLONE_SYSVSEM child and the calling process share a single list of  System  V  semaphore  adjustment (semadj) values (see semop(2))
// CLONE_THREAD the child is placed in the same thread group as the calling  process