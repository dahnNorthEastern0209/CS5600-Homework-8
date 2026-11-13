#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "common_threads.h"

#include "main-header.h"
#include "vector-header.h"

// taken from https://en.wikipedia.org/wiki/Fetch-and-add
int fetch_and_add(int * variable, int value) {
        return __atomic_fetch_add(variable, value, __ATOMIC_SEQ_CST);
}

void vector_add(vector_t *v_dst, vector_t *v_src) {
    int i;
    for (i = 0; i < VECTOR_SIZE; i++) {
	fetch_and_add(&v_dst->values[i], v_src->values[i]);
    }
}

extern vector_t v[];

void fini() {
    printf("v0[0] = %d, v0[50] = %d, v0[99] = %d\n",
           v[0].values[0], v[0].values[50], v[0].values[99]);

    printf("v1[0] = %d, v1[50] = %d, v1[99] = %d\n",
           v[1].values[0], v[1].values[50], v[1].values[99]);
}


#include "main-common.c"

