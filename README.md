# Maga Kim
24 April 2019

This user level thread library implementation uses non-local gotos via jmp_buf, setjmp, longjmp to iterate through threads on a priority schedule, which allocates 50 ms of run time before running the next thread. 

A struct ThreadControlBlock is created to hold information about the threads while a class Schedule manages the ThreadControlBlocks.

Update 3/1/19: Added implementation of pthread_join as well as semaphore support.

Update 4/24/19: Added information on semaphores and pthread_join

## thread_control_block.cc
Implements a struct ThreadControlBlock which holds information about a thread.

	int status:
		The status of the thread indicated by const ints kReady, kRunning, kZombie, kBlockedJoin, kWaiting, and kExited.

	pthread_t *thread_id:
		The thread_id for the current thread.
	
	jmp_buf jump_buffer:
		The jmp_buf for the thread's state information

	char *stack_ptr:
		A stack_ptr pointing to a memory block (char[32767]).
	
	void *return_value:
		Pointer to hold the exit value of a thread.
		
	pthread_t join_on_thread_id:
		The thread_id of the thread the current thread is kBlockJoin on.

## schedule.cc
Implements Schedule class, a singly linked list with member functions that allow it to act as a queue.
A struct Node is defined within the class which holds the value of a ThreadControlBlock pointer.
The Schedule contains member variables pointing to the first and last node.
It also contains a pthread_t variable curr_thread_id_ that is assigned to a pushed ThreadControlBlock and incremented afterwards.

	void Schedule::push(ThreadControlBlock *thread_control_block, pthread_t *restrict_thread):
		Pushes a ThreadControlBlock pointer into the Schedule with a pthread_t pointer indicated by the parameter (pthread_t *restrict_thread) within pthread_create.
		If a nullptr is passed for the pthread_t pointer than a new pthread_t pointer is created for the created ThreadControlBlock.

	void Schedule::update_first():
		If the first element's status is kRunning then change the status to kReady.
		Move the first element of the queue to the back.

	ThreadControlBlock *Schedule::front():
		Returns the ThreadControlBlock pointer at the front of the queue.
		
	void Schedule::schedule_next():
		update_first() until the first element of the schedule is a valid thread to run on.
			kReady: change status to kRunning and run the thread.
			kWaiting or kZombie: update_first().
			kExited: pop().
			kBlockedJoin: check if targeted thread has completed. If it has then change the targeted thread status from kZombie to kExited and the current thread from kBlockedJoin to kRunning.
			
	ThreadControlBlock *Schedule::get_join_thread(pthread_t join_on_thread_id):
		Iterate through the schedule to get a pointer to the target/terminating thread.
		
	bool Schedule::check_threads_exited():
		Check to see if all other threads have a status of either kZombie or kExited. The main thread should be the only one calling this function.
	
pop(), push(), empty() are pretty self explanatory.

## thread.cc
	int pthread_create(pthread_t *restrict_thread, const pthread_attr_t *restrict_attr,
		void *(*start_routine)(void*), void *restrict_arg):
		Creates a new ThreadControlBlock pointer and pushes restrict_arg and the address to pthread_exit_wrapper onto the block's stack (32763 and 32759 respectively).
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
		
	void lock():
		Blocks signals to schedule_handler.
	
	void unlock():
		Unblocks signals to schedule_handler.
		
	void pthread_exit_wrapper():
		Stores the exit value in a pointer and passes to pthread_exit.

	void pthread_exit(void *value_ptr):
		Checks if the current thread is the one created for main.
			Main:
				If check_threads_exited() returns true then exit(0).
				If false then signal schedule_handler.
			Other:
				Change status to kZombie, store value_ptr in return_value, and signal schedule_handler.

	pthread_t pthread_self(void):
		Returns the value pointed to by the current thread's thread_id.

	void schedule_handler(int signum):
		Sets the jump_buffer of the current thread, calls schedule_next() on the schedule, and longjmps to the next thread on Schedule (now at the front).
	
	int pthread_join(pthread_t thread, void **value_ptr)
		Return -1 if pthread_t thread does not match any thread_id in the schedule.
		If the target thread has a status of kZombie then change it to kExited and assign to value_ptr the exit value (from return_value).
		Otherwise change status to kBlockedJoin and take exit value of target thread whenever this status changes.
		
## semaphore_queue.cc
The SemaphoreQueue holds ThreadControlBlock pointers corresponding to the threads that are waiting on the semaphore.

## semaphore.cc
Each semaphore has a SemaphoreQueue holding waiting threads.

	int sem_init(sem_t *sem, int pshared, unsigned value):
		Initialize semaphore and its SemaphoreQueue and assign to id the passed value.
	
	int sem_wait(sem_t *sem):
		If id is 0 then change the status of the running thread to kWaiting, push it on to the SemaphoreQueue, and signal schedule_handler.
		Otherwise decrement id.
		
	int sem_post(sem_t *sem):
		Increment id. If there is a thread waiting at the front of the SemaphoreQueue then change its status to kReady and pop it off.

## threads.cc
Unity file includes thread.cc, schedule.cc, thread_control_block.cc, semaphore.cc, and semaphore_queue.cc for makefile to create threads.o

## Issues that I ran into
Not having a clear understanding of jmp_buf and set_jmp:
The man pages helped me as well as: http://www.csl.mtu.edu/cs4411.ck/www/NOTES/non-local-goto/goto.html

Allocating restrict_arg and pthread_exit on the stack:
Reviewed the call stack.

Not returning correct values on pthread_self and when printing &thread:
Changed the ThreadControlBlock's thread_id to a pointer value which uses the pthread_t* input.


