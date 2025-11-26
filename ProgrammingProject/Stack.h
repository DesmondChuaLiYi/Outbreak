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
		// Get the last element using iterator
		auto it = fElements.begin();
		T value;
		int currentIndex = 0;
		int lastIndex = size() - 1;
		
		// Iterate to the last element
		while (currentIndex < lastIndex) {
			++it;
			++currentIndex;
		}
		value = *it;
		
		// Remove the last element
		fElements.popBack();
		return value;
	}
};

#endif /* STACK_H */