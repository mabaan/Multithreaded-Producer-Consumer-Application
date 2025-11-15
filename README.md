# Multithreaded Producer–Consumer Application

## Table of Contents
1. [Project Overview](#project-overview)
2. [Features](#features)
3. [Code Structure](#code-structure)
4. [Compilation](#compilation)
5. [Running the Program](#running-the-program)
6. [Sample Test Cases](#sample-test-cases)
7. [Bonus Features](#bonus-features)
8. [Group Details](#group-details)

---

## Project Overview

This mini project implements a multithreaded producer–consumer system in C using the POSIX `pthread` and `semaphore` libraries.

Multiple producer threads generate random integers and insert them into a shared **bounded circular buffer**, while multiple consumer threads remove and process these items. The project focuses on:

- Correct synchronization using **mutexes** and **semaphores**
- Blocking behavior when the buffer is full or empty
- Avoiding race conditions and deadlocks
- **Graceful termination** using the **Poison Pill** technique
- Optional **priority handling** and **performance metrics** for bonus credit

The code is written to be readable and modular for an undergraduate Operating Systems course.

---

## Features

### Core requirements

- Shared bounded buffer with fixed capacity
- Circular queue behavior for both producer and consumer sides
- Multiple producer threads
- Multiple consumer threads
- Blocking when:
  - Buffer is full (producers wait)
  - Buffer is empty (consumers wait)
- Mutual exclusion using a single `pthread_mutex_t`
- No busy waiting and no `sleep` used for synchronization
- Input validation for all command line arguments

### Termination with Poison Pill

- Each producer creates a fixed number of items (constant `ITEMS_PER_PRODUCER`, set to 20)
- After all producers finish, the main thread inserts **one poison pill per consumer**
- Poison pills are special items that cause consumers to exit their loop
- Poison pills are not counted as real items in the statistics

---

## Code Structure

The repository is organized as follows:

```text
include/
    common.h        # Shared types and function declarations

src/
    buffer.h        # Interface for shared buffer operations
    buffer.c        # Implementation of circular buffer and metrics
    producer.c      # Producer thread function
    consumer.c      # Consumer thread function
    producer_consumer.c  # Thread creation, joining, and summary
    main.c          # Argument parsing and entry point

tests/
    sample_run.sh   # Build script and two sample runs for the report
