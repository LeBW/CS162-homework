#include <stdio.h>
#include <stdlib.h>
#include "wq.h"

void* push(wq_t *work_queue) {
    printf("[child] Start to push element to queue.\n");
    wq_push(work_queue, 1);
    wq_push(work_queue, 2);
    printf("[child] Push succeed.\n");
    return NULL;
}

int main(int argc, char **argv) {
    wq_t work_queue;
    wq_init(&work_queue);
    pthread_t thread;
    pthread_create(&thread, NULL, &push, &work_queue);
    // pthread_yield();
    printf("[parent] start to pop from queue\n");
    wq_pop(&work_queue);
    printf("[parent] pop finished\n");
    // wq_init(&work_queue);
    wq_pop(&work_queue);
    
    return 0;
}
