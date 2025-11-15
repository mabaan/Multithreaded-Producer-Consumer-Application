/*
 * File: main.c
 * Description:
 *   Entry point for the program.
 *   Parses and validates command-line arguments (number of producers, consumers, and buffer size).
 *   Creates and joins threads, initializes random seed, and manages clean termination.
 *   Handles errors such as invalid inputs or failed thread creation.
 */

#include <errno.h>
#include <limits.h>
#include "buffer.h"

/* Parse a strictly positive int from a string.
   Returns -1 on any error. */
static int parse_positive_int(const char *text)
{
    char *endptr = NULL;
    errno = 0;
    long value = strtol(text, &endptr, 10);

    if (errno != 0 || endptr == text || *endptr != '\0') {
        return -1;
    }
    if (value <= 0 || value > INT_MAX) {
        return -1;
    }
    return (int)value;
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr,
                "Usage: %s <num_producers> <num_consumers> <buffer_size>\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    int producers = parse_positive_int(argv[1]);
    int consumers = parse_positive_int(argv[2]);
    int buffer_size = parse_positive_int(argv[3]);

    if (producers <= 0 || consumers <= 0 || buffer_size <= 0) {
        fprintf(stderr,
                "All arguments must be positive integers greater than zero.\n");
        return EXIT_FAILURE;
    }

    return run_simulation(producers, consumers, buffer_size);
}
