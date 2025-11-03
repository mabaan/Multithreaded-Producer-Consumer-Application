/*
 * File: producer.c
 * Description:
 *   Defines the logic executed by each producer thread.
 *   Each producer generates random integer data and adds it to the shared buffer.
 *   Automatically blocks if the buffer is full.
 *   Stops after producing a fixed number of items and signals completion.
 */
