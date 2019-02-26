// Maga Kim
// 11 February 2019

#include "thread.h"

void init() {
	scheduling = true;
	ThreadControlBlock *thread_control_block = new ThreadControlBlock();
	thread_control_block->status = thread_control_block->kRunning;
	sched.push(thread_control_block, nullptr);
	
	struct sigaction timeout;
	timeout.sa_handler = schedule_handler;
	sigemptyset(&timeout.sa_mask);
	timeout.sa_flags = SA_NODEFER | SA_RESTART; // SA_NODEFER is chosen to prevent signals from blocking within the handler.
	if (sigaction(SIGALRM, &timeout, NULL) == -1) {
		exit(errno);
	}

	sigemptyset(&alarm_set);
	sigaddset(&alarm_set, SIGALRM);

	ualarm(50000, 50000); // Start a 50 ms timer for priority scheduling.
}

void lock() {
	sigprocmask(SIG_BLOCK, &alarm_set, NULL);
}

void unlock() {
	sigprocmask(SIG_UNBLOCK, &alarm_set, NULL);
}

void schedule_handler(int signum) {
	ThreadControlBlock *thread_control_block = sched.front();
        if (!setjmp(thread_control_block->jump_buffer)) { // If this thread has been longjmped to it will not run this block.
				// Move the first thread to back of schedule and jump to next thread.
                sched.update_first();
                longjmp(sched.front()->jump_buffer, 1);
        }
}

// Create and push a ThreadControlBlock pointer for the new thread.
// Push the address of restrict_arg and pthread_exit onto the ThreadControlBlock's stack.
// Set the ThreadControlBlock's jmp_buf stack pointer to point to pthread_exit on its stack.
// Set the ThreadControlBlock's jmp_buf program counter to point to the address of start routine.
int pthread_create(pthread_t *restrict_thread, const pthread_attr_t *restrict_attr,
                void *(*start_routine)(void*), void *restrict_arg) {
	
	if (!scheduling) { // If this is the first thread to be created then create and push aThreadControlBlock for main then start timer.
		init();
	}
	lock();
	ThreadControlBlock *thread_control_block = new ThreadControlBlock();
	
	// Set up the stack for the newly created ThreadControlBlock.
	char *ptr = thread_control_block->stack_ptr;
	*(int*)(ptr+32763) = (int)restrict_arg;
	*(int*)(ptr+32759) = (int)pthread_exit; // Push the address for pthread_exit on the space below so the thread returns to pthread_exit.

	// Set up the newly created ThreadControlBlock's jmp_buf so it can run start_routine and exit to pthread_create.
	setjmp(thread_control_block->jump_buffer);
	
	thread_control_block->jump_buffer[0].__jmpbuf[thread_control_block->kJumpBufStackPtr] = 
		ptr_mangle((uintptr_t)(ptr+32759));
	thread_control_block->jump_buffer[0].__jmpbuf[thread_control_block->kJumpBufProgCounter] = 
		ptr_mangle((uintptr_t)start_routine);
	sched.push(thread_control_block, restrict_thread);

	unlock();
	return *(restrict_thread);
}

int pthread_join(pthread_t thread, void **value_ptr) {
	return 0;	
}

void pthread_exit(void *value_ptr) {
	if (sched.front()->thread_id != 0) {
		sched.pop();
		ThreadControlBlock *thread_control_block = sched.front();
                longjmp(thread_control_block->jump_buffer, 1);
        }
	exit(0);
}


pthread_t pthread_self(void) {
	return *(sched.front()->thread_id); //  The current thread is always at the front of the Schedule.
}

// Mangle for jmp_buf on 32 bit system.
static int ptr_mangle(int p)
{
    unsigned int ret;
    asm(" movl %1, %%eax;\n"
        " xorl %%gs:0x18, %%eax;"
        " roll $0x9, %%eax;"
        " movl %%eax, %0;"
    : "=r"(ret)
    : "r"(p)
    : "%eax"
    );
    return ret;
}
