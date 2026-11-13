#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "common_threads.h"

sem_t s; 

void *child(void *arg) {
    sleep(1);
 
    printf("child\n");
    // use semaphore here
    sem_post(&s);

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;

    sem_init(&s, 0, 0);

    printf("parent: begin\n");
    // init semaphore here
    Pthread_create(&p, NULL, child, NULL);

    sem_wait(&s);
    // use semaphore here
    printf("parent: end\n");
    
    pthread_join(p, NULL);
    sem_destroy(&s);

    return 0;
}

