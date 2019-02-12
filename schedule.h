// Maga Kim
// 11 February 2019

#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "thread_control_block.h"
#include <cerrno>
#include <cstdio>

// Schedule holds ThreadControlBlock* for priority scheduling.
class Schedule {
	public:
		Schedule();
		~Schedule();
		void push(ThreadControlBlock *t, pthread_t *p);
		void pop();
		void update_first(); // Move the first element to the back.
		bool empty();
		ThreadControlBlock *front();
	private:
		struct Node {
			Node() {
				value = nullptr;
				next = nullptr;
			}
			~Node() {
				delete value;
			}
                        ThreadControlBlock *value;
                        Node *next;
                };

		pthread_t current_thread_id_; // current_thread_id_ is assigned to a pushed ThreadControlBlock and increments afterwards.
		Node *first_;
		Node *last_;
};

#endif // SCHEDULE_H
