// Maga Kim
// 26 February 2019

#include "semaphore_queue.h"

SemaphoreQueue::SemaphoreQueue() {
	this->first_ = nullptr;
	this->last_ = nullptr;
}

SemaphoreQueue::~SemaphoreQueue() {
	Node *node = this->first_;
        while (this->first_) {
                this->first_ = this->first_->next;
                delete node;
                node = this->first_;
        }
}

void SemaphoreQueue::push(ThreadControlBlock *thread_control_block) {
	Node *node = new Node(thread_control_block);
	if (!this->first_) {
		this->first_ = node;
		this->last_ = node;
	}
	else {
		this->last_->next = node;
		this->last_ = this->last_->next;
	}
}

void SemaphoreQueue::pop() {
	if (!this->first_) return;
	if (this->first_ == this->last_) {
                delete this->first_;
                this->first_ = nullptr;
                this->last_ = nullptr;
        }
        else {
                Node *node = this->first_;
                this->first_ = this->first_->next;
                delete node;
        }
}

ThreadControlBlock *SemaphoreQueue::front() {
	return (this->first_) ? this->first_->value : nullptr;
}
