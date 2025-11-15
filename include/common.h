/*
 * File: common.h
 * Description:
 *   Contains shared definitions, macros, and data structures used across all source files.
 *   Defines the BoundedBuffer struct, constants (e.g., POISON_PILL), and includes
 *   required headers for pthreads, semaphores, and standard I/O.
 */


#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define ITEMS_PER_PRODUCER 20
#define POISON_PILL_VALUE -1

/* Item stored in the shared buffer */
typedef struct {
    int value;
    int is_poison;
    int priority;               /* 0 normal, 1 urgent */
    struct timespec enqueue_time;   /* used for latency */
} Item;

/* Shared buffer struct */
typedef struct {
    Item *urgent_slots;
    Item *normal_slots;

    int capacity;

    int urgent_head;
    int urgent_tail;
    int urgent_count;

    int normal_head;
    int normal_tail;
    int normal_count;

    int real_items_target;
    int real_items_seen;

    /* Bonus metrics */
    long long total_latency_ns;
    long long latency_samples;
    int has_first_enqueue;
    struct timespec first_enqueue;
    struct timespec last_dequeue;

    sem_t slots_free;
    sem_t slots_used;
    pthread_mutex_t lock;
} SharedBuffer;

/* Arguments for threads */
typedef struct {
    int id;
    SharedBuffer *buffer;
} ThreadArgs;

/* Thread functions */
void *producer_thread(void *arg);
void *consumer_thread(void *arg);

/* Main simulation entry */
int run_simulation(int producer_count,
                   int consumer_count,
                   int buffer_capacity);

#endif

