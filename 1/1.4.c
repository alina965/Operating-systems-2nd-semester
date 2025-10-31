#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void cleanup(void* arg) {
    char* arr = (char*)arg;
    if (arr) {
        free(arr);
    }

    printf("line freed\n");
}

void *mythread(void *arg) {
	printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());

    // a)
    /*while (1) {
        printf("hellooooo\n");
    }*/

    // b)
    /*int num = 1;
    while (1) {
        num++;
        pthread_testcancel();
    }*/

    // c)
    char *line = malloc(20 * sizeof(char));

    if (line == NULL) {
        printf("main: malloc() failed for string\n");
        return NULL;
    }
    strcpy(line, "hello world");

    pthread_cleanup_push(cleanup, line);

    while (1) {
        printf("%s\n", line);
    }

    pthread_cleanup_pop(0);

    free(line);

	return NULL;
}

int main() {
	pthread_t tid;
	int err;

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	err = pthread_create(&tid, NULL, mythread, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    sleep(3);
    pthread_cancel(tid);
    printf("Thread was cancelled\n");

    err = pthread_join(tid, NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

	return 0;
}
