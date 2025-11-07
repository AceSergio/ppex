#include "malloc.h"

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define ALIGNMENT 16
#define PAGE_SIZE 4096

typedef struct block
{
    size_t size;
    int free; // 1 si libre, 0 si occupé
    struct block *next;
    void *data;

} block_t;

static block_t *head = NULL;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static size_t align_size(size_t size)
{
    size_t r = size % ALIGNMENT;

    if (r == 0)
    {
        return size;
    }
    size_t padding = ALIGNMENT - r;
    return size + padding;
}

static size_t get_page_size(size_t size)
{
    size_t required = align_size(size) + sizeof(block_t);

    if (required > PAGE_SIZE)
    {
        size_t r = size % PAGE_SIZE;

        if (r == 0)
        {
            return size;
        }

        size_t padding = PAGE_SIZE - r;

        return size + padding;
    }

    return PAGE_SIZE;
}

static block_t *allocate_new_page(size_t size)
{
    size_t alloc_size = get_page_size(size);

    void *page_addr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (page_addr == MAP_FAILED)
    {
        return NULL;
    }

    block_t *new_block = (block_t *)page_addr;
    new_block->size = alloc_size - sizeof(block_t);
    new_block->free = 0;
    new_block->data = (void *)(new_block + 1);

    new_block->next = head;
    head = new_block;

    return new_block;
}

static block_t *find_free_block(size_t size)
{
    block_t *current = head;
    while (current)
    {
        if (current->free && current->size >= size)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

__attribute__((visibility("default"))) void *malloc(size_t size)
{
    if (size == 0)
        return NULL;

    pthread_mutex_lock(&mutex);

    size_t aligned_size = align_size(size);
    block_t *block = find_free_block(aligned_size);
    if (!block)
    {
        block = allocate_new_page(aligned_size);
        if (!block)
        {
            pthread_mutex_unlock(&mutex);
            return NULL;
        }
    }
    else
    {
        if (block->size >= aligned_size + sizeof(block_t) + ALIGNMENT)
        {
            block_t *new_free_block = block->data + aligned_size;
            new_free_block->size = block->size - aligned_size - sizeof(block_t);
            new_free_block->free = 1;
            new_free_block->next = block->next;
            new_free_block->data = (new_free_block + 1);

            block->size = aligned_size;
            block->next = new_free_block;
        }
        block->free = 0;
    }
    pthread_mutex_unlock(&mutex);
    return block->data;
}

__attribute__((visibility("default"))) void free(void *ptr)
{
    if (!ptr)
        return;

    pthread_mutex_lock(&mutex);
    block_t *current = head;

    while (current)
    {
        if (current->data == ptr)
        {
            current->free =
                1; // optimisation ? fusion des blocs adjacents libres
            break;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&mutex);
}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size)
{
    if (!ptr)
        return malloc(size);
    if (size == 0)
    {
        free(ptr);
        return NULL;
    }
    pthread_mutex_lock(&mutex);

    block_t *current = head;
    while (current && current->data != ptr)
    {
        current = current->next;
    }
    if (!current)
    {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    if (size <= current->size)
    {
        current->size = align_size(size);
        pthread_mutex_unlock(&mutex);
        return ptr;
    }
    else
    {
        void *new_ptr = malloc(size);
        if (new_ptr)
        {
            memcpy(new_ptr, ptr, current->size);
            free(ptr);
        }
        pthread_mutex_unlock(&mutex);
        return new_ptr;
    }
}

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    if (nmemb == 0 || size == 0)
        return NULL;

    if (size > SIZE_MAX / nmemb)
        return NULL;

    size_t total_size = nmemb * size;

    void *ptr = malloc(total_size);

    if (ptr)
    {
        memset(ptr, 0, total_size);
    }
    return ptr;
}
