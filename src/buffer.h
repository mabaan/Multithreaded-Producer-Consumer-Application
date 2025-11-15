/*
 * File: buffer.h
 * Description:
 *   Header file for the bounded buffer implementation.
 *   Declares buffer structure, constants, and function prototypes.
 *   Included by producer, consumer, and main modules for shared access.
 */

#ifndef BUFFER_H
#define BUFFER_H

#include "common.h"

/* Initialize shared buffer and its synchronization objects.
   Returns 0 on success, -1 on failure. */
int buffer_init(SharedBuffer *buf,
                int capacity,
                int total_real_items);

/* Free memory and destroy semaphores and mutex. */
void buffer_destroy(SharedBuffer *buf);

/* Insert one item into the circular buffer (blocks if full). */
void buffer_put(SharedBuffer *buf, Item item);

/* Remove one item from the buffer (blocks if empty). */
Item buffer_get(SharedBuffer *buf);

#endif /* BUFFER_H */
