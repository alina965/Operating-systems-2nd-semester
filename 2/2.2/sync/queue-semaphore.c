#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>
#include <semaphore.h>

#include "queue.h"

sem_t is_full;
sem_t is_empty;
pthread_mutex_t mutex;

void *qmonitor(void *arg) {
	queue_t *q = (queue_t *)arg;

	printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

	while (1) {
		queue_print_stats(q);
		sleep(1);
	}

	return NULL;
}

queue_t* queue_init(int max_count) {
	printf("QUEUE WITH SEMAPHORE\n");
	int err;

	if ((err = pthread_mutex_init(&mutex, NULL)) != 0) {
		printf("queue_init: pthread_mutex_init() failed: %s\n", strerror(err));
		abort();
	}

	if ((err = sem_init(&is_empty, 0, max_count)) != 0) {
		printf("queue_init: sem_init failed: %s\n", strerror(err));
		abort();
	}
	if ((err = sem_init(&is_full, 0, 0)) != 0) {
		printf("queue_init: sem_init failed: %s\n", strerror(err));
		abort();
	}

	queue_t *q = malloc(sizeof(queue_t));
	if (!q) {
		printf("Cannot allocate memory for a queue\n");
		abort();
	}

	q->first = NULL;
	q->last = NULL;
	q->max_count = max_count;
	q->count = 0;

	q->add_attempts = q->get_attempts = 0;
	q->add_count = q->get_count = 0;

	err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
	if (err) {
		printf("queue_init: pthread_create() failed: %s\n", strerror(err));
		abort();
	}

	return q;
}

void queue_destroy(queue_t *q) {
    int err;

	if ((err = pthread_mutex_destroy(&mutex)) != 0) {
		printf("queue_destroy: pthread_mutex_destroy() failed: %s\n", strerror(err));
		abort();
	}

    if ((err = sem_destroy(&is_empty)) != 0) {
		printf("queue_destroy: sem_destroy() failed: %s\n", strerror(err));
		abort();
	}
	if ((err = sem_destroy(&is_full)) != 0) {
		printf("queue_destroy: sem_destroy() failed: %s\n", strerror(err));
		abort();
	}

	pthread_cancel(q->qmonitor_tid);

	err = pthread_join(q->qmonitor_tid, NULL);
	if (err) {
		printf("queue_destroy: pthread_join() failed: %s\n", strerror(err));
		abort();
	}

	qnode_t *current = q->first;
    while (current != NULL) {
        qnode_t *next = current->next;
        free(current);
        current = next;
    }

	free(q);
	printf("Memory for queue freed!\n");
}

int queue_add(queue_t *q, int val) {
	sem_wait(&is_empty);
	pthread_mutex_lock(&mutex);

	q->add_attempts++;

	assert(q->count <= q->max_count);

	qnode_t *new = malloc(sizeof(qnode_t));
	if (!new) {
		printf("Cannot allocate memory for new node\n");
		abort();
	}

	new->val = val;
	new->next = NULL;

	if (!q->first)
		q->first = q->last = new;
	else {
		q->last->next = new;
		q->last = q->last->next;
	}

	q->count++;
	q->add_count++;

	pthread_mutex_unlock(&mutex);
    sem_post(&is_full);

	return q->add_attempts + q->get_attempts;
}

int queue_get(queue_t *q, int *val) {
	sem_wait(&is_full);
	pthread_mutex_lock(&mutex);

	q->get_attempts++;

	assert(q->count >= 0);

	qnode_t *tmp = q->first;

	*val = tmp->val;
	q->first = q->first->next;

	free(tmp);
	q->count--;
	q->get_count++;

	pthread_mutex_unlock(&mutex);
    sem_post(&is_empty);
	
	return q->add_attempts + q->get_attempts;
}

void queue_print_stats(queue_t *q) {
	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		q->count,
		q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
		q->add_count, q->get_count, q->add_count -q->get_count);
}