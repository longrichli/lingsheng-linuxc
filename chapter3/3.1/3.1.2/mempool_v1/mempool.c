/*
des:
    分配长度不固定的内存池

    +——————+————————————+. 
  +-| node |            |
  | +------+            |
  | |                   |
  | |                   |
  | |                   |
  | +--------------------
  |
  | +——————+————————————+
  +>| node |            |
    +------+            |
    |                   |
    |                   |
    |                   |
    +--------------------
*/

#include <stdio.h>
#include <stdlib.h>
#define MEM_PAGE_SIZE (4096)

typedef struct mp_node_s {
    void *free_ptr;
    void *end;
    size_t size;
    struct mp_node_s *next;
} mp_node_t;

typedef struct mempool_s {
    mp_node_t *first;
} mempool_t;


int mp_init(mempool_t *mp, size_t size) {
    if(!mp) return -1;
    if(size <= 0) {
        size = MEM_PAGE_SIZE;
    }
    void *addr = malloc(size);
    if(!addr) return -1;
    mp_node_t *node = (mp_node_t *)addr;
    node->end = addr + size;
    node->free_ptr = addr + sizeof(mp_node_t);
    node->size = size;
    node->next = NULL;
    mp->first = node;
    return 0;
}

void mp_destory(mempool_t *mp) {
    if(!mp || !mp->first) return;
    mp_node_t *node = mp->first;
    while(node) {
        mp_node_t *tmp_node = node->next;
        free(node);
        node = tmp_node;
    }
}

void* mp_alloc(mempool_t *mp, size_t size) {
    if(!mp || !mp->first) return NULL;
    mp_node_t *node = mp->first;
    while(node) {
        if(node->end - node->free_ptr >= size) {
            /* can alloc */
            void *tmp_ptr = node->free_ptr;
            node->free_ptr += size;
            return tmp_ptr;
        }
        node = node->next;
    }
    size_t new_size = 0;
    /* can not alloc, new node */
    if(size <= MEM_PAGE_SIZE - sizeof(mp_node_t)) {
        new_size = MEM_PAGE_SIZE;
    } else {
        new_size = size + sizeof(mp_node_t);
    }

    void *addr = NULL;
    addr = malloc(new_size);
    if(!addr) return NULL;
    node = (mp_node_t *)addr;
    node->free_ptr = addr + sizeof(mp_node_t);
    node->end = addr + new_size;
    node->next = mp->first->next;
    mp->first = node;
    node->size = new_size;

    void *tmp_addr = node->free_ptr;
    node->free_ptr += size;
    return tmp_addr;
}

#include <string.h>
int main(int argc, char **argv) {
    mempool_t mp;
    mp_init(&mp, 4096);
    char *p = mp_alloc(&mp, 16);
    strcpy(p, "1");
    char *p1 = mp_alloc(&mp, 32);
    strcpy(p1, "2");
    char *p2 = mp_alloc(&mp, 64);
    strcpy(p2, "3");
    char *p3 = mp_alloc(&mp, 128);
    strcpy(p3, "4");
    char *p4 = mp_alloc(&mp, 4096 - 32);
    strcpy(p4, "5");
    char *p5 = mp_alloc(&mp, 4096);
    strcpy(p5, "6");

    printf("%p, %s\n", p, p);
    printf("%p, %s\n", p1, p1);
    printf("%p, %s\n", p2, p2);
    printf("%p, %s\n", p3, p3);
    printf("%p, %s\n", p4, p4);
    printf("%p, %s\n", p5, p5);
    mp_destory(&mp);

    return 0;
}