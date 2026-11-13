#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "common_threads.h"

//
// Here, you have to write (almost) ALL the code. Oh no!
// How can you show that a thread does not starve
// when attempting to acquire this mutex you build?
//

typedef struct waiter {
    sem_t ready;
    struct waiter *next;
} waiter_t;


typedef struct __ns_mutex_t {
    sem_t guard;
    int locked;
    waiter_t *head;
    waiter_t *tail;
} ns_mutex_t;

void ns_mutex_init(ns_mutex_t *m) {
    sem_init(&m->guard, 0, 1);

    m->locked = 0;
    m->head = NULL;
    m->tail = NULL;
}

void ns_mutex_acquire(ns_mutex_t *m) {
    waiter_t self;

    sem_init(&self.ready, 0, 0);
    self.next = NULL;

    sem_wait(&m->guard);

    if (m->locked == 0) {
        m->locked = 1;

        sem_post(&m->guard);
    } else {
        if (m->tail == NULL) {
            m->head = &self;
        } else {
            m->tail->next = &self;
        }

        m->tail = &self;

        sem_post(&m->guard);

        sem_wait(&self.ready);
    }

    sem_destroy(&self.ready);
}

void ns_mutex_release(ns_mutex_t *m) {
    sem_wait(&m->guard);

    if (m->head != NULL) {
        waiter_t *next_thread = m->head;

        m->head = next_thread->next;

        if (m->head == NULL) {
            m->tail = NULL;
        }

        sem_post(&next_thread->ready);
    } else {
        m->locked = 0;
    }

    sem_post(&m->guard);
}

ns_mutex_t mutex;
int counter = 0;

typedef struct {
    int id;
    int loops;
} thread_arg_t;

void *worker(void *arg) {
    thread_arg_t *t = (thread_arg_t *)arg;

    for (int i = 0; i < t->loops; i++) {
        printf("Thread %d: requesting mutex\n", t->id);

        ns_mutex_acquire(&mutex);

        printf("Thread %d: acquired mutex\n", t->id);

        counter++;

        usleep(20000);

        printf("Thread %d: releasing mutex\n", t->id);

        ns_mutex_release(&mutex);

        usleep(1000);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    int num_threads = 4;
    int loops = 3;

    pthread_t threads[num_threads];
    thread_arg_t args[num_threads];

    ns_mutex_init(&mutex);

    printf("parent: begin\n");

    for (int i = 0; i < num_threads; i++) {
        args[i].id = i;
        args[i].loops = loops;

        Pthread_create(&threads[i], NULL, worker, &args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        Pthread_join(threads[i], NULL);
    }

    printf("Final counter: %d\n", counter);
    printf("parent: end\n");

    sem_destroy(&mutex.guard);

    return 0;
}

