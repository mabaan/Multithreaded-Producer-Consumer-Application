/*
 * File: producer_consumer.c
 * Description:
 *   Main driver file for the multithreaded Producer-Consumer application.
 *   Initializes shared buffer, semaphores, and mutex.
 *   Creates producer and consumer threads, handles synchronization, and ensures
 *   proper termination using the Poison Pill technique.
 *   Ensures no race conditions or deadlocks occur.
 */
