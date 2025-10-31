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

    free(data->line);
    free(data);

    return NULL;
}

int main() {
    pthread_t thr;
    int err;

    struct structura* data = malloc(sizeof(struct structura));
    if (data == NULL) {
        printf("main: malloc() failed for struct\n");
        return -1;
    }
    
    data->line = malloc(20 * sizeof(char));
    if (data->line == NULL) {
        printf("main: malloc() failed for string\n");
        free(data);
        return -1;
    }
    
    strcpy(data->line, "privet");
    data->num = 9;

    printf("Contents of the structure inside the main:\nline field: %s\nnum field: %d\n", data->line, data->num);

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    err = pthread_create(&thr, NULL, mythread, data);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        free(data->line);
        free(data);
        return -1;
    }

    err = pthread_detach(thr);
    if (err) {
        printf("main: pthread_detach() failed: %s\n", strerror(err));
        free(data->line);
        free(data);
        return -1;
    }
    
    sleep(1);

    return 0;
}
