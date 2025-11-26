#ifndef SINGLYLINKEDNODEITERATOR_H
#define SINGLYLINKEDNODEITERATOR_H
#include "SinglyLinkedNode.h"

template <class T>
class SinglyLinkedNodeIterator {
private:
	typedef SinglyLinkedNode<T> Node;
	Node* current;

public:
	// Constructor
	SinglyLinkedNodeIterator(Node* startNode) : current(startNode) {}

	// Constructor to access the sentinel
	SinglyLinkedNodeIterator(Node& node) {
		current = &node;
	}

	// Pre-increment operator
	SinglyLinkedNodeIterator& operator++() {
		current = current->next;
		return *this;
	}

	// Post-increment operator
	SinglyLinkedNodeIterator operator++(int) {
		SinglyLinkedNodeIterator temp = *this;
		++(*this);
		return temp;
	}

	// Dereference operator
	T operator*() const {
		return current->value;
	}

	// Equality operator
	bool operator==(const SinglyLinkedNodeIterator& other) const {
		return current == other.current;
	}

	// Inequality operator
	bool operator!=(const SinglyLinkedNodeIterator& other) const {
		return current != other.current;
	}

	// Get current node
	Node* getCurrent() {
		return current;
	}
};

#endif /* SINGLYLINKEDNODEITERATOR_H */