#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

struct structura {
    char* line;
    int num;
};

void *mythread(void *arg) {
    printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());
    struct structura* data = (struct structura*)arg;

    printf("Contents of the structure inside the stream:\nline field: %s\nnum field: %d\n", data->line, data->num);

    int *num = malloc(sizeof(int));
    *num = 9;

    return num;
}

int main() {
    pthread_t thr;
    int err;

    struct structura data = {"privet", 9};

    printf("Contents of the structure inside the main:\nline field: %s\nnum field: %d\n", data.line, data.num);

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());
    err = pthread_create(&thr, NULL, mythread, &data);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    int *res;
    err = pthread_join(thr, (void**)&res);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

    printf("result = %d\n", *res);

    free(res);

    return 0;
}
