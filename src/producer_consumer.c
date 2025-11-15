/*
 * File: producer_consumer.c
 * Description:
 *   Main driver file for the multithreaded Producer-Consumer application.
 *   Initializes shared buffer, semaphores, and mutex.
 *   Creates producer and consumer threads, handles synchronization, and ensures
 *   proper termination using the Poison Pill technique.
 *   Ensures no race conditions or deadlocks occur.
 */

#include "buffer.h"

/* Start all producer and consumer threads and manage poison pills.
   Returns EXIT_SUCCESS on success, EXIT_FAILURE on any error. */
int run_simulation(int producer_count,
                   int consumer_count,
                   int buffer_capacity)
{
    int total_real_items = producer_count * ITEMS_PER_PRODUCER;

    SharedBuffer shared;
    if (buffer_init(&shared, buffer_capacity, total_real_items) != 0) {
        fprintf(stderr, "Failed to initialize shared buffer.\n");
        return EXIT_FAILURE;
    }

    pthread_t *producers = malloc(sizeof(pthread_t) * producer_count);
    pthread_t *consumers = malloc(sizeof(pthread_t) * consumer_count);

    if (producers == NULL || consumers == NULL) {
        fprintf(stderr, "Failed to allocate thread arrays.\n");
        free(producers);
        free(consumers);
        buffer_destroy(&shared);
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));

    /* Create producer threads */
    for (int i = 0; i < producer_count; i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        if (args == NULL) {
            fprintf(stderr, "Failed to allocate producer args.\n");
            free(producers);
            free(consumers);
            buffer_destroy(&shared);
            return EXIT_FAILURE;
        }
        args->id = i + 1;
        args->buffer = &shared;

        if (pthread_create(&producers[i], NULL, producer_thread, args) != 0) {
            perror("pthread_create producer");
            free(args);
            free(producers);
            free(consumers);
            buffer_destroy(&shared);
            return EXIT_FAILURE;
        }
    }

    /* Create consumer threads */
    for (int i = 0; i < consumer_count; i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        if (args == NULL) {
            fprintf(stderr, "Failed to allocate consumer args.\n");
            free(producers);
            free(consumers);
            buffer_destroy(&shared);
            return EXIT_FAILURE;
        }
        args->id = i + 1;
        args->buffer = &shared;

        if (pthread_create(&consumers[i], NULL, consumer_thread, args) != 0) {
            perror("pthread_create consumer");
            free(args);
            free(producers);
            free(consumers);
            buffer_destroy(&shared);
            return EXIT_FAILURE;
        }
    }

    /* Wait for all producers to finish */
    for (int i = 0; i < producer_count; i++) {
        pthread_join(producers[i], NULL);
    }

    /* Insert one poison pill per consumer after producers have stopped.
       Because the buffer is FIFO and no more real items are added,
       all real items are consumed before these pills. */
    for (int i = 0; i < consumer_count; i++) {
        Item pill;
        pill.value = POISON_PILL_VALUE;
        pill.is_poison = 1;
        buffer_put(&shared, pill);
    }

    /* Wait for all consumers to exit after receiving their pill */
    for (int i = 0; i < consumer_count; i++) {
        pthread_join(consumers[i], NULL);
    }

    printf("\nSummary:\n");
    printf("  Total real items expected: %d\n", shared.real_items_target);
    printf("  Total real items consumed: %d\n", shared.real_items_seen);

    free(producers);
    free(consumers);
    buffer_destroy(&shared);

    if (shared.real_items_seen != shared.real_items_target) {
        fprintf(stderr,
                "Warning: mismatch between produced and consumed item counts.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
