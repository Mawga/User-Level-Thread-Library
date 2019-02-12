// Maga Kim
// 11 February 2019

#include "thread_control_block.h"

ThreadControlBlock::ThreadControlBlock() {
	this->status = this->kReady;
	this->stack_ptr = new int[8191]; // Use int because we are working with 32 bit addresses. Initialize the stack for ~32767 bytes.
}

ThreadControlBlock::~ThreadControlBlock() {
	delete[] this->stack_ptr;
}

