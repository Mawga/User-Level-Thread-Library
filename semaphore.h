// Maga Kim
// 26 February 2019

#include <semaphore.h>
#include "thread.h"
#include "semaphore_queue.h"

typedef struct {
	unsigned id;
	
	SemaphoreQueue *queue;
} __sem_t;

int sem_init(sem_t *sem, int pshared, unsigned value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_post(sem_t *sem);

