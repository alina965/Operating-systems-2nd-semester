#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#define NUM 5

int global_var = 666;

void *mythread(void *arg) {
        int var = 555;
        int static static_var = 444;
        int const const_var = 777;

        printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());

        pthread_t self_id = pthread_self();
        printf("pthread_self() = %lu\n", self_id);

        printf("global_var = %p; tid = %d\n", &global_var, gettid()); // совпадают
        printf("var = %p; tid = %d\n", &var, gettid()); // разные
        printf("static_var = %p; tid = %d\n", &static_var, gettid()); // совпадают
        printf("const_var = %p; tid = %d\n", &const_var, gettid()); // разные для потоков 

        var++;
        global_var++;

        printf("var = %d; global_var = %d\n", var, global_var);
        return NULL;
}

int main() {
        pthread_t thr[NUM + 1];
        int err;

        printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	for (int i = 0; i < NUM + 1; i++) {
        	err = pthread_create(&thr[i], NULL, mythread, NULL);
                printf("new thread #%d %ld\n", i, thr[i]);
        	if (err) {
            		printf("main: pthread_create() failed: %s\n", strerror(err));
                	return -1;
        	}
	}

	for(int i = 0; i < NUM + 1; i++) {
               pthread_join(thr[i], NULL);
        }

        return 0;
}
