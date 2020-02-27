/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines.
 */

#include "mm_alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// size of stuct metadata.
static unsigned metasize = sizeof(struct metadata);
// header of the linked list.
struct metadata* header = NULL;

void *mm_malloc(size_t size) {
    /* YOUR CODE HERE */
    if (size == 0)
    {
        return NULL;
    }

    struct metadata* cur = header;
    struct metadata* prev = NULL;
    while (cur)
    {
        if (cur->free && (cur->size - size >= 0))
        {
            int extra_space = cur->size - size;
            if (extra_space > metasize)
            {
                //TODO: enough to create a new block.

            }
            cur->size = size;
            cur->free = false;
            memset(cur->contents, 0, size);

            return cur->contents;
        }
        prev = cur;
        cur = cur->next;
    }
    // no sufficiently large free block is found, use sbrk to create more space on the heap.
    if ((cur = sbrk(metasize + size)) == (void *)-1)
    {
        printf("sbrk error\n");
        return NULL;
    }
    cur->prev = prev;
    cur->next = NULL;
    cur->free = false;
    cur->size = size;
    memset(cur->contents, 0, size);
    if (header == NULL) // first time to allocate memory
        header = cur;
    return cur->contents;
}

void *mm_realloc(void *ptr, size_t size) {
    /* YOUR CODE HERE */
    return NULL;
}

void mm_free(void *ptr) {
    /* YOUR CODE HERE */
    if (ptr == NULL)
        return;
    struct metadata *cur = header;
    while (cur)
    {
        //printf("cur->contents, ptr: %lu, %lu\n", cur->contents, ptr);
        if (cur->contents == ptr)
        {
            cur->free = true;
            printf("free succeed: %lu\n", ptr);
            //TODO: coalesce consecutive free blocks.
        }
        cur = cur->next;
    }
}

// test
int main()
{
    printf("size of metadata: %u\n", metasize);
    mm_malloc(sizeof(int));
    return 0;
}
