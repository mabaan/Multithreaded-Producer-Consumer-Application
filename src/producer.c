/*
 * File: producer.c
 * Description:
 *   Defines the logic executed by each producer thread.
 *   Each producer generates random integer data and adds it to the shared buffer.
 *   Items are marked as normal or urgent priority. About 25 percent of items are urgent.
 *   Automatically blocks if the buffer is full and stops after a fixed number of items.
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

        /* Mark roughly 25 percent of items as urgent (priority 1) */
        int urgent_flag = (rand() % 4 == 0);
        item.priority = urgent_flag ? 1 : 0;

        buffer_put(buf, item);

        printf("[Producer-%d] Produced item: %d (priority %d)\n",
               id,
               item.value,
               item.priority);
    }

    printf("[Producer-%d] Finished producing %d items.\n",
           id, ITEMS_PER_PRODUCER);

    free(info);
    return NULL;
}
