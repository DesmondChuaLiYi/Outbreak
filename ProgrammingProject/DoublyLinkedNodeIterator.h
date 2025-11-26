#ifndef DOUBLYLINKEDNODEITERATOR_H
#define DOUBLYLINKEDNODEITERATOR_H
#include "DoublyLinkedNode.h"

template <class DataType>
class DoublyLinkedNodeIterator {
private:
	typedef DoublyLinkedNode<DataType> Node;
	Node* current;

public:
	DoublyLinkedNodeIterator(Node* startNode) {
		current = startNode;
	}

	// New constructor to access the sentinel in the iterator's end() method
	DoublyLinkedNodeIterator(Node& node) {
		current = &node;
	}

	// Pre-increment operator
	DoublyLinkedNodeIterator& operator++() {
		current = current->next;
		return *this;
	}

	// Post increment operator
	DoublyLinkedNodeIterator operator++(int) {
		DoublyLinkedNodeIterator temp = *this;
		++(*this);
		return temp;
	}

	// Pre-decrement operator
	DoublyLinkedNodeIterator& operator--() {
		current = current->previous;
		return *this;
	}

	// Post-decrement operator
	DoublyLinkedNodeIterator operator--(int) {
		DoublyLinkedNodeIterator temp = *this;
		--(*this);
		return temp;
	}

	bool operator==(const DoublyLinkedNodeIterator& other) const {
		return current == other.current;
	}

	bool operator!=(const DoublyLinkedNodeIterator& other) const {
		return current != other.current;
	}

	DataType operator*() const {
		return current->value;
	}

	Node* getCurrent() {
		return current;
	}

	DoublyLinkedNodeIterator begin() {
		return DoublyLinkedNodeIterator(current);
	}

	DoublyLinkedNodeIterator end() {
		return DoublyLinkedNodeIterator(Node::NIL); // Using NIL at the end marker
	}
};

#endif /* DOUBLYLINKEDNODEITERATOR */