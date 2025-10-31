#ifndef MYTHREAD_H
#define MYTHREAD_H

#define _GNU_SOURCE
#include <sys/types.h>

typedef int mythread_t;

int my_pthread_create(mythread_t *thread, void *(start_routine), void *arg);

#endif