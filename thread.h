// Maga Kim
// 11 February 2019

#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <cstdint>
#include "schedule.h"

bool scheduling = false;
Schedule sched;

// Create ThreadControlBlock pointer for main and push onto the Schedule.
// Begin a timer for priority scheduling.
void init();

// Create a new ThreadControlBlock and push it onto the Schedule.
int pthread_create(pthread_t *restrict_thread, const pthread_attr_t *restrict_attr,
		void *(*start_routine)(void*), void *restrict_arg);

// If the current thread (the front of the Schedule) is the one created for main, exit program.
// Otherwise run the next thread in the Schedule.
void pthread_exit(void *value_ptr);

// Return the thread id associated with current ThreadControlBlock.
pthread_t pthread_self(void);

// Mangle pointer for jmp_buf for 32 bit addresses.
static int ptr_mangle(int p);

// On SIGALRM run the next thread.
void schedule_handler(int signum);
#endif // THREAD_H
