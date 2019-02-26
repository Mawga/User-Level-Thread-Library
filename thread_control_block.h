// Maga Kim
// 11 February 2019

#ifndef THREAD_CONTROL_BLOCK_H
#define THREAD_CONTROL_BLOCK_H

#include "setjmp.h"
#include <cstdlib>

// #define STACK_SIZE 32767;
// #define MAX_NUM_THREADS 128;

struct ThreadControlBlock {
        ThreadControlBlock();
        ~ThreadControlBlock();
        int status;
	pthread_t *thread_id;
	jmp_buf jump_buffer;
	char *stack_ptr;

	// Status of threads.
	const int kReady = 0;
	const int kRunning = 1;
	const int kExited = 2;

	// Indicies to jmp_buf's stack pointer and program counter.
	const int kJumpBufStackPtr = 4;
	const int kJumpBufProgCounter = 5;
};

#endif // THREAD_CONTROL_BLOCK_H

