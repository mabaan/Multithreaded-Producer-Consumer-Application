/*
 * File: consumer.c
 * Description:
 *   Defines the logic executed by each consumer thread.
 *   Continuously removes data from the shared buffer and processes it (e.g., prints to console).
 *   Blocks if the buffer is empty.
 *   Terminates gracefully upon receiving the special POISON_PILL value.
 */
