#include <stdio.h>
#include <unistd.h>
#include "my-thread.h"

void *worker_function(void *arg) {
    int thread_num = *(int *)arg;
    printf("Поток %d запущен\n", thread_num);
    
    for (int i = 0; i < 3; i++) {
        printf("Поток %d: шаг %d\n", thread_num, i);
        sleep(1);
    }
    
    printf("Поток %d завершен\n", thread_num);
    return NULL;
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0); // убирает буферизацию

    mythread_t thread1, thread2;
    int arg1 = 1, arg2 = 2;
    
    printf("Создаем потоки...\n");
    
    if (my_pthread_create(&thread1, worker_function, &arg1) == 0) {
        printf("Поток 1 создан успешно, ID: %d\n", thread1);
    } 
    else {
        printf("Ошибка создания потока 1\n");
    }

    if (my_pthread_create(&thread2, worker_function, &arg2) == 0) {
        printf("Поток 2 создан успешно, ID: %d\n", thread2);
    } 
    else {
        printf("Ошибка создания потока 2\n");
    }
    
    sleep(5);
    printf("Основной поток завершен\n");
    
    return 0;
}