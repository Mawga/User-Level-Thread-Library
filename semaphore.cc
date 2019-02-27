// Maga Kim
// 26 February 2019
//
#include "semaphore.h"
int sem_init(sem_t *sem, int pshared, unsigned value) {
	lock();
	if (value < 0) {
		unlock();
		return -1;
	}
	__sem_t *redefined_sem = new __sem_t;
	redefined_sem->queue = new SemaphoreQueue;
	redefined_sem->id = value;
	(*sem).__align = (long int)(redefined_sem);
	unlock();
	return 0;
}

int sem_destroy(sem_t *sem) {
	lock();
	__sem_t *redefined_sem = (__sem_t*)((*sem).__align);
	delete redefined_sem->queue;
	delete redefined_sem;
	(*sem).__align = (long int)(nullptr);
	unlock();
	return 0;
}

int sem_wait(sem_t *sem) {
	lock();
	__sem_t *redefined_sem = (__sem_t*)((*sem).__align);
	if (redefined_sem->id == 0) {
		sched.front()->status = sched.front()->kWaiting;
		redefined_sem->queue->push(sched.front());
		unlock();
		kill(getpid(), SIGALRM);
		lock();
	}
	--(redefined_sem->id);
	unlock();
	return 0;
}

int sem_post(sem_t *sem) {
	lock();
	__sem_t *redefined_sem = (__sem_t*)((*sem).__align);
	++(redefined_sem->id);

	ThreadControlBlock *thread_control_block = redefined_sem->queue->front();
	if (!thread_control_block) {
		unlock();
		return 0;
	}
	thread_control_block->status = thread_control_block->kReady;
	redefined_sem->queue->pop();
	unlock();
	return 0;
}
