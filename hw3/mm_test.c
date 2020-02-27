#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

/* Function pointers to hw3 functions */
void* (*mm_malloc)(size_t);
void* (*mm_realloc)(void*, size_t);
void (*mm_free)(void*);

void load_alloc_functions() {
    void *handle = dlopen("hw3lib.so", RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    char* error;
    mm_malloc = dlsym(handle, "mm_malloc");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    mm_realloc = dlsym(handle, "mm_realloc");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    mm_free = dlsym(handle, "mm_free");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }
}

void test_mm_alloc()
{
    int *data = (int*) mm_malloc(sizeof(int)*10);
    //int *data = malloc(sizeof(int));
    assert(data != NULL);
    data[0] = 0x162;
    data[1] = 888;
    printf("data addr: %p\n", data);
    mm_free(data);
    int *data1 = (int*) mm_malloc(sizeof(int));
    int *data2 = (int*) mm_malloc(sizeof(int));
    int *data3 = (int*) mm_malloc(sizeof(int));
    printf("data1 addr: %p, data2 addr: %p, data3 addr: %p\n", data1, data2, data3);
    printf("malloc test successful!\n");
}

void test_mm_free()
{
    int *data1 = (int*) mm_malloc(sizeof(int));
    int *data2 = (int*) mm_malloc(sizeof(int));
    int *data3 = (int*) mm_malloc(sizeof(int));
    mm_free(data1);
    mm_free(data3);
    mm_free(data2);
    int *data = (int*) mm_malloc(60);
    printf("data1 addr: %p, data addr: %p\n", data1, data);
}

int main() {
    load_alloc_functions();

//    test_mm_alloc();
    test_mm_free();

    return 0;
}
