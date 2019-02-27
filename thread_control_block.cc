// Maga Kim
// 11 February 2019

#include "thread_control_block.h"

ThreadControlBlock::ThreadControlBlock() {
	this->status = this->kReady;
	this->stack_ptr = new char[kStackSize]; // Use int because we are working with 32 bit addresses. Initialize the stack for ~32767 bytes.
	this->thread_id = nullptr;
	this->return_value = nullptr;
}

ThreadControlBlock::~ThreadControlBlock() {
	delete[] this->stack_ptr;
}

