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
	if (node->value->status == node->value->kRunning) node->value->status = node->value->kReady;
	if (this->first_->value->status == this->first_->value->kReady) this->first_->value->status = this->first_->value->kRunning;
}

void Schedule::schedule_next() {
	Schedule::update_first();
	while (this->first_->value->status != this->first_->value->kRunning) {
		ThreadControlBlock *thread = this->first_->value;
		if (thread->status == thread->kExited) {
			Schedule::update_first();
		}
		else if (thread->status == thread->kZombie) {
			Schedule::pop();
		}
		else if (thread->status == thread->kBlockedJoin) {
			ThreadControlBlock *join_on_thread = get_join_thread(thread->join_on_thread_id);
			if (!join_on_thread) exit(1);
			if (join_on_thread->status == join_on_thread->kExited) {
				join_on_thread->status = join_on_thread->kZombie;
				thread->status = thread->kRunning;
				thread->return_value = join_on_thread->return_value;
			}
			else {
				Schedule::update_first();
			}
		}
	}
}

ThreadControlBlock *Schedule::get_join_thread(pthread_t join_on_thread_id) {
	Node *node = this->first_->next;
	while (node != nullptr) {
		if (*(node->value->thread_id) == join_on_thread_id) return node->value;
		node = node->next;
	}
	return nullptr;
}
bool Schedule::check_threads_exited() {
	Node *node = this->first_->next; // Don't check main thread's kRunning status.
	while (node != nullptr) {
		if (node->value->status != node->value->kExited && node->value->status != node->value->kZombie) return false;
		node = node->next;
	}
	return true;
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
