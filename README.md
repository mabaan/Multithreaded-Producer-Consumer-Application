# Multithreaded Producer Consumer Application

## 1. Project overview

This mini project implements a multithreaded producer consumer system in C using the POSIX pthread and semaphore libraries.

Several producer threads generate random integers and insert them into a shared circular bounded buffer. Several consumer threads remove items from the buffer and print them. The shared buffer uses semaphores and a mutex to prevent race conditions and to block threads when the buffer is full or empty. The project also implements a poison pill termination scheme and two optional bonus features: item priority and timing metrics.

The code is written to be clear and easy to follow for an undergraduate Operating Systems course.

Main goals:

- Practice working with pthreads, semaphores and mutexes  
- Correctly implement a bounded buffer producer consumer pattern  
- Avoid race conditions, busy waiting and deadlocks  
- Terminate all threads safely using a poison pill  
- Optionally measure latency and throughput and implement priority handling


## 2. Features

### Core requirements

- Shared bounded buffer with fixed capacity  
- Circular queue behavior using head and tail indices  
- Multiple producer threads and multiple consumer threads  
- Producers block when the buffer is full  
- Consumers block when the buffer is empty  
- Mutual exclusion enforced with a single pthread mutex  
- No busy waiting and no use of sleep for synchronization  
- Input validation for all command line arguments  
- Clean termination using the poison pill technique

### Poison pill termination

- Each producer generates a fixed number of items  
- After all producers finish, the main thread inserts one poison pill per consumer  
- Consumers exit their main loop when they read a poison pill  
- Poison pills are not counted as real items  
- The program prints a short summary showing how many real items were consumed

### Bonus features

The project implements both optional bonus features.

1. **Priority handling**

   - Each real item has a `priority` field  
     - `0` for normal items  
     - `1` for urgent items  
   - Producers randomly mark about 25 percent of items as urgent  
   - The shared buffer stores urgent and normal items in two internal circular queues  
   - When consumers remove an item, urgent items are always taken first  
   - FIFO order is preserved inside each priority level  
   - The console output shows the priority of each produced and consumed item

2. **Throughput and latency metrics**

   - For every real item, the program records  
     - The time at which a producer inserts the item into the buffer  
     - The time at which a consumer removes it  
   - Average latency is computed as the mean of all dequeue time minus enqueue time values  
   - Throughput is computed as  
     `(number of real items consumed) / (time between first enqueue and last dequeue)`  
   - The final summary prints both average latency (milliseconds) and throughput (items per second)  
   - Two suggested runs are included in the test script to support the comparison table in the report


## 3. Repository structure and file description

This section explains what each file does and how the pieces fit together.

### Header and shared definitions

- `include/common.h`  
  Defines project wide types and constants used across all source files.  
  Contains:
  - `ITEMS_PER_PRODUCER` and `POISON_PILL_VALUE` constants  
  - `Item` struct, which stores:
    - integer value  
    - poison flag  
    - priority flag (normal or urgent)  
    - enqueue timestamp for latency measurement  
  - `SharedBuffer` struct, which stores:
    - two circular queues (`urgent_slots` and `normal_slots`)  
    - queue indices and counts for both priority levels  
    - target and actual counts of real items  
    - timing fields for latency and throughput  
    - semaphores for free and used slots  
    - mutex to protect all shared state  
  - `ThreadArgs` struct, used to pass an id and a pointer to `SharedBuffer` into each thread  
  - Function prototypes for the producer and consumer thread functions and for `run_simulation`

### Buffer module

- `src/buffer.h`  
  Declares the buffer interface:
  - `buffer_init` to initialize the buffer and its synchronization objects  
  - `buffer_destroy` to free memory and destroy semaphores and the mutex  
  - `buffer_put` to insert an item into the buffer (blocking if full)  
  - `buffer_get` to remove an item from the buffer (blocking if empty)

- `src/buffer.c`  
  Implements the shared circular buffer and metric collection.
  Main responsibilities:
  - Allocate memory for the urgent and normal queues  
  - Initialize indices, counters, semaphores and the mutex  
  - Enforce global capacity using `slots_free` and `slots_used` semaphores  
  - On `buffer_put`:
    - If the item is real, record its enqueue time  
    - Store urgent items in the urgent queue and others in the normal queue  
    - Always hold the mutex while modifying indices and counters  
  - On `buffer_get`:
    - Remove from the urgent queue if it is non empty, otherwise from the normal queue  
    - If the item is real, update:
      - count of real items seen  
      - total latency and sample count  
      - time of the last dequeue  
    - Release the mutex before posting to `slots_free`

This module hides all details of how the buffer is organized. Producers and consumers only interact through the two functions `buffer_put` and `buffer_get`.

### Producer and consumer modules

- `src/producer.c`  
  Defines the function executed by each producer thread.  
  Responsibilities:
  - Generate `ITEMS_PER_PRODUCER` random integers  
  - For each item:
    - Assign a random priority  
      - roughly 25 percent urgent, 75 percent normal  
    - Mark it as a real item (not a poison pill)  
    - Call `buffer_put` to insert it into the shared buffer  
  - Print a message for each produced item showing its value and priority  
  - Print a final message when the producer finishes  
  - Free the `ThreadArgs` structure before returning

- `src/consumer.c`  
  Defines the function executed by each consumer thread.  
  Responsibilities:
  - Repeatedly call `buffer_get` to remove items from the buffer  
  - If the item is a poison pill, print a message and exit the loop  
  - If the item is real, print its value and priority  
  - Free the `ThreadArgs` structure before returning

Priority handling is enforced by the buffer. The consumer simply requests the next available item and processes whatever it gets.

### Simulation controller and entry point

- `src/producer_consumer.c`  
  Coordinates the whole simulation.  
  Responsibilities:
  - Compute the total number of real items (`producers * ITEMS_PER_PRODUCER`)  
  - Initialize the shared buffer with that target and the given capacity  
  - Allocate arrays of pthread identifiers for producers and consumers  
  - Create producer threads, each with its own `ThreadArgs`  
  - Create consumer threads in the same way  
  - Join all producer threads  
  - After all producers finish, insert one poison pill into the buffer for each consumer  
  - Join all consumer threads  
  - After everything is done, print a summary:
    - expected number of real items  
    - actual number of real items consumed  
    - average latency and throughput if enough data is available  
  - Free all allocated memory and destroy the buffer  
  - Return `EXIT_SUCCESS` on success and `EXIT_FAILURE` on any error

This file contains the logic that directly matches the project specification: creation of threads, poison pills, and final statistics.

- `src/main.c`  
  The program entry point.  
  Responsibilities:
  - Validate the number of command line arguments  
  - Parse the number of producers, number of consumers and buffer size  
  - Reject invalid or non positive values  
  - Call `run_simulation` with the parsed arguments  
  - Propagate the return code from `run_simulation`

### Test script

- `tests/sample_run.sh`  
  Simple shell script that compiles the project and runs two test cases.  
  Responsibilities:
  - Compile all source files into an executable named `producer_consumer`  
  - Run:
    - a stress test with a small buffer size (for example `4 4 2`)  
    - a test with a larger buffer size (for example `4 4 32`)  
  - The outputs from these runs can be used to fill the latency and throughput table in the report

The script is optional but convenient for quick testing.


## 4. Compilation

To compile the program manually, run:

```bash
gcc -Wall -Wextra -std=c11 \
    -Iinclude -Isrc \
    src/main.c \
    src/buffer.c \
    src/producer.c \
    src/consumer.c \
    src/producer_consumer.c \
    -pthread \
    -o producer_consumer
