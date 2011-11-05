#ifndef _MEM_ALLOCATE_H
#define _MEM_ALLOCATE_H

#include <malloc.h>

extern __inline__ void* mem_alloc (size_t size) {
    return malloc(size);
}

extern __inline__ void* mem_calloc (size_t count, size_t size) {
    return calloc(count, size);
}

extern __inline__ void* mem_realloc (void *p, size_t size) {
    return realloc(p, size);
}

extern __inline__ void* mem_align (size_t a, size_t size) {
    #ifdef __wii__
    return memalign(a, size);
    #else
    return malloc(size);
    #endif
}

extern __inline__ void mem_free (void* mem) {
    free(mem);
}

#endif /* _MEM_ALLOCATE_H */
