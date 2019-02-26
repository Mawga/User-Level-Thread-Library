// Maga Kim
// 11 February 2019

#include "schedule.h"

Schedule::Schedule() {
	this->first_ = nullptr;
	this->last_ = nullptr;
	this->current_thread_id_ = 0;
}

Schedule::~Schedule() {
	Node *node = this->first_;
	while (node) {
		Node *temp_node = node;
		node = node->next;
		delete temp_node;
	}
}

void Schedule::push(ThreadControlBlock *thread_control_block, pthread_t *restrict_thread) {
	Node *node = new Node();
	
	// If a thread is not given, create new thread pointer for the ThreadControlBlock.
	// Assign current_thread_id_ to its thread_id.
	if (restrict_thread) thread_control_block->thread_id = restrict_thread;
	else thread_control_block->thread_id = new pthread_t;
	*(thread_control_block->thread_id) = current_thread_id_++;

	if (!node) {
		exit(errno);
	}
	node->value = thread_control_block;
	node->next = nullptr;
	if (!(this->first_)) { // The Schedule is empty, assign first_ and last_ to the pushed element.
		this->first_ = this->last_ = node;
		return;
	}
	(this->last_)->next = node;
	this->last_ = node;
}

void Schedule::update_first() {
	if (!(this->first_)) {
                exit(1);
        }
	
	if (this->first_ == this->last_) return;
	
	Node *node = this->first_;

	
	this->first_ = this->first_->next;
	node->next = nullptr;

	this->last_->next = node;
	this->last_ = node;
	node->value->status = node->value->kReady;
	this->first_->value->status = this->first_->value->kRunning;
}

void Schedule::pop() {
	if (!(this->first_)) {
		exit(1);
	}
	// When popping with only one element in the Schedule set first_ and last_ to nullptr.
	if (this->first_ == this->last_) {
		delete this->first_;
		this->first_ = this->last_ = nullptr;
		return;
	}
	Node *node = this->first_;
	this->first_ = this->first_->next;
	delete node;
}

bool Schedule::empty() {
	return (this->first_) ? false : true;
}

ThreadControlBlock *Schedule::front() {
	if (!(this->first_)) {
		exit(1);
	}
	return this->first_->value;
}
