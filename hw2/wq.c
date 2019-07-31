#include <stdlib.h>
#include "wq.h"
#include "utlist.h"

/* Initializes a work queue WQ. */
void wq_init(wq_t *wq) {

  /* TODO: Make me thread-safe! */
  pthread_mutex_init(&wq->mut, NULL);
  pthread_cond_init(&wq->cond, NULL);
  pthread_mutex_lock(&wq->mut);
  wq->size = 0;
  wq->head = NULL;
  pthread_mutex_unlock(&wq->mut);
}

/* Remove an item from the WQ. This function should block until there
 * is at least one item on the queue. */
int wq_pop(wq_t *wq) {

  /* TODO: Make me blocking and thread-safe! */
  pthread_mutex_lock(&wq->mut);
  while (wq->size <= 0) {
    pthread_cond_wait(&wq->cond, &wq->mut);
  }
  wq_item_t *wq_item = wq->head;
  int client_socket_fd = wq->head->client_socket_fd;
  wq->size--;
  DL_DELETE(wq->head, wq->head);

  free(wq_item);
  pthread_mutex_unlock(&wq->mut);
  return client_socket_fd;
}

/* Add ITEM to WQ. */
void wq_push(wq_t *wq, int client_socket_fd) {

  /* TODO: Make me thread-safe! */
  pthread_mutex_lock(&wq->mut);
  wq_item_t *wq_item = calloc(1, sizeof(wq_item_t));
  wq_item->client_socket_fd = client_socket_fd;
  DL_APPEND(wq->head, wq_item);
  wq->size++;
  if (wq->size > 0) {
    pthread_cond_broadcast(&wq->cond);
  }
  pthread_mutex_unlock(&wq->mut);
}
