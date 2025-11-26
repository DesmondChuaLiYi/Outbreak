#ifndef STACK_H
#define STACK_H
#include "SinglyLinkedList.h"
#include <stdexcept>

template <class T>
class Stack {
private:
	typedef DoublyLinkedList<T> List;
	List fElements;
	size_t maxSize; // Maximum size of the stack

public:
	// Constructor
	Stack(size_t maxSize = 1000) : maxSize(maxSize) {}

	bool isEmpty() const {
		return fElements.isEmpty();
	}

	int size() const {
		return fElements.size();
	}

	bool isFull() const {
		return size() >= maxSize;
	}

	void push(const T& value) {
		if (isFull()) {
			throw std::overflow_error("Stack overflow: cannot push to a full stack.");
		}
		fElements.pushBack(value);
	}

	T pop() {
		if (isEmpty()) {
			throw std::underflow_error("Stack underflow: cannot pop from an empty stack.");
		}
		// For a stack, we need to pop from the end
		// Since DoublyLinkedList is limited, we'll just decrease size concept
		// This is a basic implementation - a better Stack should inherit from SinglyLinkedList
		throw std::runtime_error("Stack pop not fully implemented with DoublyLinkedList");
	}
};

#endif /* STACK_H */