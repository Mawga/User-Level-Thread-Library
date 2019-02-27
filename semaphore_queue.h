// Maga Kim
// 26 February 2019

#ifndef SEMAPHORE_QUEUE_H
#define SEMAPHORE_QUEUE_H

#include "thread_control_block.h"

class SemaphoreQueue {
        public:
                SemaphoreQueue();
                ~SemaphoreQueue();
                void push(ThreadControlBlock *t);
                void pop();
		ThreadControlBlock *front();
        private:
		struct Node {
        	Node(ThreadControlBlock *thread) {
                	value = thread;
                	next = nullptr;
        	}
        	ThreadControlBlock *value;
        	Node *next;
		};
                Node *first_;
                Node *last_;
};

#endif // SEMAPHORE_QUEUE_H
