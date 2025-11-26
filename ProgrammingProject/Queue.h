#ifndef QUEUE_H
#define QUEUE_H
#include "SinglyLinkedList.h"
#include <stdexcept>

template <class T>
class Queue {
private:
	typedef SinglyLinkedList<T> List;
	List fElements;
	size_t maxSize; // Maximum size of the queue

public:
	// Constructor
	Queue(size_t maxSize = 1000) : maxSize(maxSize) {}

	bool isEmpty() const {
		return fElements.isEmpty();
	}

	int size() const {
		return fElements.size();
	}

	bool isFull() const {
		return size() >= maxSize;
	}

	void enqueue(const T& value) {
		if (isFull()) {
			throw std::overflow_error("Queue overflow: cannot enqueue to a full queue.");
		}
		fElements.pushBack(value);
	}

	T dequeue() {
		if (isEmpty()) {
			throw std::underflow_error("Queue underflow: cannot dequeue from an empty queue.");
		}
		// Get the front element before removing it
		T frontElement = *fElements.begin();
		fElements.popFront();
		return frontElement;
	}
};

#endif /* QUEUE_H */