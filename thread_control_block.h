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
	pthread_t join_on_thread_id;
	void *return_value;
	jmp_buf jump_buffer;
	char *stack_ptr;

	// Status of threads.
	const int kReady = 0;
	const int kRunning = 1;
	const int kExited = 2;
	const int kBlockedJoin = 3;
	const int kZombie = 4;
	const int kWaiting = 5;

	// Indicies to jmp_buf's stack pointer and program counter.
	const int kJumpBufStackPtr = 4;
	const int kJumpBufProgCounter = 5;

	const int kStackSize = 32767;
};

#endif // THREAD_CONTROL_BLOCK_H

