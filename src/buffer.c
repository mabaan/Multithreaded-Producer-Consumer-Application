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

    buf->urgent_slots = malloc(sizeof(Item) * capacity);
    buf->normal_slots = malloc(sizeof(Item) * capacity);

    if (buf->urgent_slots == NULL || buf->normal_slots == NULL) {
        perror("malloc");
        free(buf->urgent_slots);
        free(buf->normal_slots);
        return -1;
    }

    buf->capacity = capacity;

    buf->urgent_head = 0;
    buf->urgent_tail = 0;
    buf->urgent_count = 0;

    buf->normal_head = 0;
    buf->normal_tail = 0;
    buf->normal_count = 0;

    buf->real_items_target = total_real_items;
    buf->real_items_seen = 0;

    buf->total_latency_ns = 0;
    buf->latency_samples = 0;
    buf->has_first_enqueue = 0;

    if (sem_init(&buf->slots_free, 0, capacity) != 0) {
        perror("sem_init slots_free");
        free(buf->urgent_slots);
        free(buf->normal_slots);
        return -1;
    }

    if (sem_init(&buf->slots_used, 0, 0) != 0) {
        perror("sem_init slots_used");
        sem_destroy(&buf->slots_free);
        free(buf->urgent_slots);
        free(buf->normal_slots);
        return -1;
    }

    if (pthread_mutex_init(&buf->lock, NULL) != 0) {
        perror("pthread_mutex_init");
        sem_destroy(&buf->slots_free);
        sem_destroy(&buf->slots_used);
        free(buf->urgent_slots);
        free(buf->normal_slots);
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

    free(buf->urgent_slots);
    free(buf->normal_slots);

    buf->urgent_slots = NULL;
    buf->normal_slots = NULL;
}

void buffer_put(SharedBuffer *buf, Item item)
{
    sem_wait(&buf->slots_free);

    pthread_mutex_lock(&buf->lock);

    if (!item.is_poison) {
        clock_gettime(CLOCK_MONOTONIC, &item.enqueue_time);

        if (!buf->has_first_enqueue) {
            buf->first_enqueue = item.enqueue_time;
            buf->has_first_enqueue = 1;
        }
    }

    if (!item.is_poison && item.priority == 1) {
        buf->urgent_slots[buf->urgent_head] = item;
        buf->urgent_head = (buf->urgent_head + 1) % buf->capacity;
        buf->urgent_count++;
    } else {
        buf->normal_slots[buf->normal_head] = item;
        buf->normal_head = (buf->normal_head + 1) % buf->capacity;
        buf->normal_count++;
    }

    pthread_mutex_unlock(&buf->lock);

    sem_post(&buf->slots_used);
}

Item buffer_get(SharedBuffer *buf)
{
    Item result;

    sem_wait(&buf->slots_used);

    pthread_mutex_lock(&buf->lock);

    if (buf->urgent_count > 0) {
        result = buf->urgent_slots[buf->urgent_tail];
        buf->urgent_tail = (buf->urgent_tail + 1) % buf->capacity;
        buf->urgent_count--;
    } else {
        result = buf->normal_slots[buf->normal_tail];
        buf->normal_tail = (buf->normal_tail + 1) % buf->capacity;
        buf->normal_count--;
    }

    if (!result.is_poison) {
        buf->real_items_seen++;

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);

        long long diff_ns =
            (now.tv_sec - result.enqueue_time.tv_sec) * 1000000000LL +
            (now.tv_nsec - result.enqueue_time.tv_nsec);

        if (diff_ns < 0) diff_ns = 0;

        buf->total_latency_ns += diff_ns;
        buf->latency_samples++;
        buf->last_dequeue = now;
    }

    pthread_mutex_unlock(&buf->lock);

    sem_post(&buf->slots_free);

    return result;
}


