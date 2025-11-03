/*
 * File: buffer.c
 * Description:
 *   Implements the circular bounded buffer used for inter-thread communication.
 *   Contains functions to initialize, insert, remove, and destroy buffer elements.
 *   Uses semaphores to track empty and full slots and a mutex to ensure mutual exclusion.
 *   Prevents race conditions and supports blocking behavior (no busy-waiting).
 */
