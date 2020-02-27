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

int main() {
    load_alloc_functions();

    int *data = (int*) mm_malloc(sizeof(int)*2);
    //int *data = malloc(sizeof(int));
    assert(data != NULL);
    data[0] = 0x162;
    data[1] = 888;
    int *data2 = (int*) mm_malloc(sizeof(int));
    //int *data2 = malloc(sizeof(int));
    printf("%d %d\n", data[0], data[1]);
    data2[0] = 1;
    mm_free(data);
    //mm_free(data2);
    int *data3 = (int*) mm_malloc(sizeof(int));
    printf("data: %lu, data3: %lu\n", data, data3);
    printf("malloc test successful!\n");
    return 0;
}
