#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void handler_for_thread (int signum) {
    printf("Поток 2 поймал сигнал SIGINT\n");
}

void *thread1(void *arg) {
	printf("Поток 1 [%d %d %d]: блокирую все сигналы\n", getpid(), getppid(), gettid());
    
    sigset_t mask;
    sigfillset(&mask);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        exit(1);
    }

    sleep(5);

	return NULL;
}

void *thread2(void *arg) {
	printf("Поток 2 [%d %d %d]: перехватываю сигнал SIGINT\n", getpid(), getppid(), gettid());

    if (signal(SIGINT, handler_for_thread) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    sleep(5);

	return NULL;
}

void *thread3(void *arg) {
    printf("Поток 3 [%d %d %d]: перехватываю сигнал SIGQUIT с помощью sigwait\n", getpid(), getppid(), gettid());

    sigset_t quit_set;
    sigemptyset(&quit_set);
    sigaddset(&quit_set, SIGQUIT);

    if (sigprocmask(SIG_BLOCK, &quit_set, NULL) == -1) { // блокирую его
        perror("sigprocmask");
        exit(1);
    }

    printf("Поток 3: жду SIGQUIT...\n");
    printf("kill -QUIT %d\n", getpid());
    
    int sig;
    int result = sigwait(&quit_set, &sig);
    
    if (result == 0 && sig == SIGQUIT) {
        printf("Поток 3: был получен сигнал SIGQUIT с помощью sigwait()\n");
    } 
    else {
        printf("Поток 3: Ошибка в sigwait(): %s\n", strerror(result));
    }

	return NULL;
}

int main() {
	pthread_t threads[4];
	int err;

    sigset_t quit_set;
    sigemptyset(&quit_set);
    sigaddset(&quit_set, SIGQUIT);

    // блокируем SIGQUIT и в этом потоке
    if (pthread_sigmask(SIG_BLOCK, &quit_set, NULL) != 0) { // МАСКА ПЕРЕДАЕТСЯ ДОЧЕРНИМ ПОТОКАМ
        perror("pthread_sigmask");
        exit(1);
    }

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	err = pthread_create(&threads[0], NULL, thread1, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    err = pthread_create(&threads[1], NULL, thread2, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    err = pthread_create(&threads[2], NULL, thread3, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    sleep(1);

    err = pthread_join(threads[0], NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_join(threads[1], NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_join(threads[2], NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

	return 0;
}
