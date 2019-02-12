# Maga Kim
11 February 2019

This user level thread library implementation uses non-local gotos via jmp_buf, setjmp, longjmp to iterate through threads on a priority schedule, which allocates 50 ms of run time before running the next thread. 

A struct ThreadControlBlock is created to hold information about the threads while a class Schedule manages the ThreadControlBlocks.

Currently there is no thread synchronization which will be implemented shortly.
# thread_control_block.cc
Implements a struct ThreadControlBlock which holds information about a thread.

	int status:
		The status of the thread indicated by const ints kReady, kRunning, kExited.

	pthread_t *thread_id:
		The thread_id for the block
	
	jmp_buf jump_buffer:
		The jmp_buf for the thread's state information

	int *stack_ptr:
		A stack_ptr pointing to a memory block (int [8191]).

# schedule.cc
Implements Schedule class, a singly linked list with member functions that allow it to act as a queue.
A struct Node is defined within the class which holds the value of a ThreadControlBlock pointer.
The Schedule contains member variables pointing to the first and last node.
It also contains a pthread_t variable curr_thread_id_ that is assigned to a pushed ThreadControlBlock and incremented afterwards.

	void push(ThreadControlBlock *t, pthread_t *p):
		Pushes a ThreadControlBlock pointer into the Schedule with a pthread_t pointer indicated by the parameter (pthread_t *restrict_thread) within pthread_create.
		If a nullptr is passed for the pthread_t pointer than a new pthread_t pointer is created for the created ThreadControlBlock.

	void update_first():
		Moves the first element of the queue to the back.
		Updates the status of the ThreadControlBlock.
			The new front now with status kRunning.
			The old front now with status kReady.

	ThreadControlBlock *front():
		Returns the ThreadControlBlock pointer at the front of the queue.

	void update_first():
		Moves the first element of the queue to the back.
	
pop(), push(), empty() are pretty self explanatory.

# thread.cc
	int pthread_create(pthread_t *restrict_thread, const pthread_attr_t *restrict_attr,
		void *(*start_routine)(void*), void *restrict_arg):
		Creates a new ThreadControlBlock pointer and pushes restrict_arg and the address to pthread_exit onto the block's stack (8190 and 8189 respectively).
		Sets the newly created ThreadControlBlock pointer's jmp_buf stack pointer to the pthread_exit's address on the block's stack.
		Sets the jmp_buf program counter to the address of start_routine.
			Uses ptr_mangle for updating the jmp_buf.
			Current implementation of ptr_mangle works on a 32 bit system or compiling with -m32.

	void init():
		init called within pthread_create when pthread_create is called for the first time.
			A conditional within pthread_create checks for a bool determining if this was the first pthread_create call.
		Pushes a ThreadControlBlock pointer for main into the schedule with a nullptr.
		Sets up a handler for SIGALRM with sigaction using flags SA_NODEFER and SA_RESTART
		Begins the timeout for priority scheduling with ualarm(50000, 50000) (50 ms).

	void pthread_exit(void *value_ptr):
		Checks if the current thread is the one created for main. 
			If so will exit program with exit(0).
			If not will pop the current thread from the Schedule and longjmp to the next thread (now at the front).

	pthread_t pthread_self(void):
		Returns the value pointed to by the current thread's thread_id.

	void schedule_handler(int signum):
		Sets the jump_buffer of the current thread, calls update_first(), and longjmps to the next thread on Schedule (now at the front).

# threads.cc
Unity file includes thread.cc, schedule.cc, and thread_control_block.cc for makefile to create threads.o

# Issues that I ran into
Not having a clear understanding of jmp_buf and set_jmp:
The man pages helped me as well as: http://www.csl.mtu.edu/cs4411.ck/www/NOTES/non-local-goto/goto.html

Allocating restrict_arg and pthread_exit on the stack:
Reviewed the call stack.

Not returning correct values on pthread_self and when printing &thread:
Changed the ThreadControlBlock's thread_id to a pointer value which uses the pthread_t* input.
    
# TODO
Implement thread synchronization

