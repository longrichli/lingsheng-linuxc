/*
des:
    分配分配固定大小块的内存池
*/
#include <stdio.h>
#include <stdlib.h>
#define PAGE_SIZE (4096)
typedef struct mempool {
    void *mem_start;
    void *free_ptr;
    size_t block_size;
    size_t free_count;
} mempool_t;

int mp_init(mempool_t *mp, size_t block_size) {
    if(!mp) return -1;
    mp->mem_start = malloc(PAGE_SIZE);
    if(!(mp->mem_start)) return -1;
    mp->free_ptr = mp->mem_start;
    mp->block_size = block_size < 16 ? 16 : block_size;
    mp->free_count = PAGE_SIZE / mp->block_size;
    
    char *ptr = (char *)mp->free_ptr;
    for(int i = 0; i < mp->free_count; i++) {
        *(char **)ptr = ptr + mp->block_size;
        ptr += mp->block_size;
    }
    return 0;
}

void *mp_alloc(mempool_t *mp) {
    if(!mp || !mp->mem_start || !mp->free_ptr || mp->free_count <= 0) {
        return NULL;
    }
    void *ptr = mp->free_ptr;
    mp->free_ptr = *(char **)mp->free_ptr;
    mp->free_count--;
    return ptr;
}

void mp_free(mempool_t *mp, void *ptr) {
    if(!mp || !ptr) return;
    *(char **)ptr = mp->free_ptr;
    mp->free_ptr = ptr;
    mp->free_count++;
}


void mp_destory(mempool_t *mp) {
    if(!mp && !mp->mem_start) {
        free(mp->mem_start);
        mp->mem_start = NULL;
    }
}

#include <string.h>
int main(int argc, char *argv) {
    mempool_t mp;
    mp_init(&mp, 32);
    
    char *p = mp_alloc(&mp);
    strcpy(p, "1");
    char *p1 = mp_alloc(&mp);
    strcpy(p1, "2");
    char *p2 = mp_alloc(&mp);
    strcpy(p2, "3");
    char *p3 = mp_alloc(&mp);
    strcpy(p3, "4");

    printf("%p, %s\n", p, p);
    printf("%p, %s\n", p1, p1);
    printf("%p, %s\n", p2, p2);
    printf("%p, %s\n", p3, p3);


    mp_free(&mp, p2);
    mp_free(&mp, p);
    mp_free(&mp, p1);
    mp_free(&mp, p3);

    p = mp_alloc(&mp);
    strcpy(p, "11");
    p1 = mp_alloc(&mp);
    strcpy(p1, "22");
    p2 = mp_alloc(&mp);
    strcpy(p2, "33");
    p3 = mp_alloc(&mp);
    strcpy(p3, "44");

    printf("%p, %s\n", p, p);
    printf("%p, %s\n", p1, p1);
    printf("%p, %s\n", p2, p2);
    printf("%p, %s\n", p3, p3);

    mp_free(&mp, p2);
    mp_free(&mp, p);
    mp_free(&mp, p1);
    mp_free(&mp, p3);


    mp_destory(&mp);


    return 0;
}