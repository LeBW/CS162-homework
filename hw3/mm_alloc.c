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

// print metadata information
void print_metadatas()
{
    struct metadata* cur = header;
    while (cur)
    {
        printf("start: %p, content size: %d, next: %p\n", cur, cur->size, cur->next);
        cur = cur->next;
    }
    printf("\n");
}

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
        if (cur->free && (cur->size >= size)) // find a free block that can accommodate 'size'
        {
            int extra_space = cur->size - size;
            if (extra_space >= metasize) // extra space is enough to create a new block.
            {
                // shrink the block size.
                cur->size = size;
                // allocate a new block.
                struct metadata* extra = (struct metadata*)(cur->contents + size);
                extra->prev = cur;
                extra->next = cur->next;
                cur->next = extra;
                if (extra->next != NULL)
                    extra->next->prev = extra;

                extra->free = true;
                extra->size = extra_space - metasize;

            }
            // don't shrink cur->size if extra space is not enouth to create a new block.
            cur->free = false;
            memset(cur->contents, 0, cur->size);
      //      printf("allocate succeed. ptr: %p, size: %d\n", cur->contents, cur->size);

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
    if (prev == NULL) // first time to allocate memory
        header = cur;
    else
        prev->next = cur;

    cur->prev = prev;
    cur->next = NULL;
    cur->free = false;
    cur->size = size;
    memset(cur->contents, 0, size);
    //printf("allocate succeed. ptr: %p, size: %d\n", cur->contents, cur->size);
    //print_metadatas();
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
        //printf("[mm_free] cur->contents, ptr: %p, %p\n", cur->contents, ptr);
        if (!cur->free && cur->contents == ptr)
        {
            cur->free = true;
            // coalesce consecutive free blocks.
            struct metadata* m = cur->next;
            while (m && m->free)
            {
                cur->size = cur->size + metasize + m->size;
                m = m->next;
            }
            cur->next = m;

            m = cur;
            while (m->prev && m->prev->free)
            {
                cur->size = cur->size + metasize + m->prev->size;
                m = m->prev;
            }
            m->size = cur->size;
            m->next = cur->next;
            //printf("free succeed. ptr: %p, size: %d\n", m->contents, m->size);

            return;
        }
        cur = cur->next;
    }
    //printf("free error. Invalid pointer address: %p\n", ptr);
}

// test
int main()
{
    printf("size of metadata: %u\n", metasize);
    mm_malloc(sizeof(int));
    return 0;
}
