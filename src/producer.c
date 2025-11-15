/*
 * File: producer.c
 * Description:
 *   Defines the logic executed by each producer thread.
 *   Each producer generates random integer data and adds it to the shared buffer.
 *   Automatically blocks if the buffer is full.
 *   Stops after producing a fixed number of items and signals completion.
 */

#include "buffer.h"

void *producer_thread(void *arg)
{
    ThreadArgs *info = (ThreadArgs *)arg;
    SharedBuffer *buf = info->buffer;
    int id = info->id;

    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        Item item;
        item.value = rand() % 100;
        item.is_poison = 0;

        buffer_put(buf, item);
        printf("[Producer-%d] Produced item: %d\n", id, item.value);
    }

    printf("[Producer-%d] Finished producing %d items.\n",
           id, ITEMS_PER_PRODUCER);

    free(info);
    return NULL;
}
