/*
 * File: buffer.c
 * Description:
 *   Implements the circular bounded buffer used for inter-thread communication.
 *   Contains functions to initialize, insert, remove, and destroy buffer elements.
 *   Uses semaphores to track empty and full slots and a mutex to ensure mutual exclusion.
 *   Prevents race conditions and supports blocking behavior (no busy-waiting).
 */

#include "buffer.h"

int buffer_init(SharedBuffer *buf,
                int capacity,
                int total_real_items)
{
    if (capacity <= 0) {
        fprintf(stderr, "Buffer capacity must be positive.\n");
        return -1;
    }

    buf->slots = malloc(sizeof(Item) * capacity);
    if (buf->slots == NULL) {
        perror("malloc");
        return -1;
    }

    buf->capacity = capacity;
    buf->head = 0;
    buf->tail = 0;

    buf->real_items_target = total_real_items;
    buf->real_items_seen = 0;

    if (sem_init(&buf->slots_free, 0, capacity) != 0) {
        perror("sem_init slots_free");
        free(buf->slots);
        return -1;
    }

    if (sem_init(&buf->slots_used, 0, 0) != 0) {
        perror("sem_init slots_used");
        sem_destroy(&buf->slots_free);
        free(buf->slots);
        return -1;
    }

    if (pthread_mutex_init(&buf->lock, NULL) != 0) {
        perror("pthread_mutex_init");
        sem_destroy(&buf->slots_free);
        sem_destroy(&buf->slots_used);
        free(buf->slots);
        return -1;
    }

    return 0;
}

void buffer_destroy(SharedBuffer *buf)
{
    if (buf == NULL) {
        return;
    }

    pthread_mutex_destroy(&buf->lock);
    sem_destroy(&buf->slots_free);
    sem_destroy(&buf->slots_used);
    free(buf->slots);

    buf->slots = NULL;
    buf->capacity = 0;
}

/* Circular enqueue */
void buffer_put(SharedBuffer *buf, Item item)
{
    /* Block until there is at least one free slot */
    sem_wait(&buf->slots_free);

    pthread_mutex_lock(&buf->lock);

    buf->slots[buf->head] = item;
    buf->head = (buf->head + 1) % buf->capacity;

    pthread_mutex_unlock(&buf->lock);

    /* Signal that a new item is available */
    sem_post(&buf->slots_used);
}

/* Circular dequeue */
Item buffer_get(SharedBuffer *buf)
{
    Item result;

    /* Block until there is at least one item */
    sem_wait(&buf->slots_used);

    pthread_mutex_lock(&buf->lock);

    result = buf->slots[buf->tail];
    buf->tail = (buf->tail + 1) % buf->capacity;

    /* Count only real items, not poison pills */
    if (!result.is_poison) {
        buf->real_items_seen++;
    }

    pthread_mutex_unlock(&buf->lock);

    /* Signal that one more free slot exists */
    sem_post(&buf->slots_free);

    return result;
}
