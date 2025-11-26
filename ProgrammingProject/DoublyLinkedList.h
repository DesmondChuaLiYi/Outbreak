#ifndef DOUBLYLINKEDLIST_H
#define DOUBLYLINKEDLIST_H
#include "DoublyLinkedNode.h"
#include "DoublyLinkedNodeIterator.h"

template <class T>
class DoublyLinkedList {
private:
	typedef DoublyLinkedNode<T> Node;
	Node* head; // Track the first node 
	Node* last; // Track the last node
	int count;

	// Helper method to get node at a specific index
	Node* getNodeAt(int index) const {
		if (index < 0 || index >= count) {
			return &Node::NIL;
		}

		Node* current = head;
		for (int i = 0; i < index; ++i) {
			current = current->next;
		}
		return current;
	}

public:
	typedef DoublyLinkedNodeIterator<T> Iterator;

	DoublyLinkedList() {
		head = &Node::NIL;
		last = &Node::NIL;
		count = 0;
	}

	~DoublyLinkedList() {
		// Safely delete all nodes
		Node* current = head;
		while (current != &(Node::NIL) && count > 0) {
			Node* next = current->next;
			delete current;
			current = next;
			--count;
		}
		head = &Node::NIL;
		last = &Node::NIL;
		count = 0;
	}

	bool isEmpty() const {
		return count == 0;
	}

	int size() const {
		return count;
	}

	// Push to tail/last
	void pushBack(const T& value) { // Renaming to pushBack for clarity
		Node* newNode = new Node(value);
		if (isEmpty()) {
			head = newNode;
			last = newNode;
		}
		else {
			last->append(newNode);
			last = newNode;
		}
		++count;
	}

	// Pop from tail/last
	void popBack() {
		if (isEmpty()) {
			return;
		}

		Node* toDelete = last;
		if (head == last) {
			// Only one node
			head = &Node::NIL;
			last = &Node::NIL;
		}
		else {
			last = last->previous;
			last->next = &Node::NIL;
		}
		delete toDelete;
		--count;
	}

	void popFront() {
		if (isEmpty()) {
			return;
		}
		Node* toDelete = head;
		if (head == last) {
			// Only one node
			head = &Node::NIL;
			last = &Node::NIL;
		}
		else {
			head = head->next;
			head->previous = &Node::NIL;
		}
		delete toDelete;
		--count;
	}

	Iterator begin() {
		return Iterator(head);
	}

	Iterator end() {
		return Iterator(Node::NIL);
	}

	Iterator begin() const {
		return Iterator(const_cast<Node*>(head));
	}

	Iterator end() const {
		return Iterator(Node::NIL);
	}
};

#endif /* DOUBLYLINKEDLIST_H */