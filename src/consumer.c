/*
 * File: consumer.c
 * Description:
 *   Defines the logic executed by each consumer thread.
 *   Continuously removes data from the shared buffer and processes it.
 *   Always consumes urgent items before normal items, while keeping FIFO order
 *   within each priority queue. Blocks if the buffer is empty and terminates
 *   gracefully upon receiving the special POISON_PILL value.
 */

#include "buffer.h"

void *consumer_thread(void *arg)
{
    ThreadArgs *info = (ThreadArgs *)arg;
    SharedBuffer *buf = info->buffer;
    int id = info->id;

    while (1) {
        Item item = buffer_get(buf);

        if (item.is_poison) {
            printf("[Consumer-%d] Received poison pill. Exiting.\n", id);
            break;
        }

        printf("[Consumer-%d] Consumed item: %d (priority %d)\n",
               id,
               item.value,
               item.priority);
    }

    free(info);
    return NULL;
}
