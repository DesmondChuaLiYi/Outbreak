#ifndef SINGLYLINKEDLIST_H
#define SINGLYLINKEDLIST_H
#include "SinglyLinkedNode.h"
#include "SinglyLinkedNodeIterator.h"

template <class T>
class SinglyLinkedList {
private:
	typedef SinglyLinkedNode<T> Node;
	Node* head; // Track the first node
	Node* last; // Track the last node
	int count;

public:
	typedef SinglyLinkedNodeIterator<T> Iterator;

	// Constructor
	SinglyLinkedList() : head(&Node::NIL), last(&Node::NIL), count(0) {}

	// Destructor
	~SinglyLinkedList() {
		Node* current = head;
		while (current != &Node::NIL) {
			Node* next = current->next;
			delete current;
			current = next;
		}
		// Reset list
		head = &Node::NIL;
		last = &Node::NIL;
		count = 0;
	}

	// Check if the list is empty
	bool isEmpty() const {
		return count == 0;
	}

	// Get list size
	int size() const {
		return count;
	}

	// Insert at last
	void pushBack(const T& value) {
		Node* newNode = new Node(value);
		if (!isEmpty()) { // List is not empty
			last->next = newNode;
			last = newNode;
		}
		else {
			head = newNode;
			last = newNode;
		}
		++count;
	}

	// Remove from back
	void popBack() {
		if (isEmpty()) {
			return; // List is empty, nothing to pop
		}

		if (head == last) { // Only one element
			delete head;
			head = &Node::NIL;
			last = &Node::NIL;
		}
		else {
			Node* current = head;

			while (current->next != last) { // Traverse to the second last node
				current = current->next;
			}

			delete last;
			last = current;
			last->next = &Node::NIL;
		}
		--count;
	}

	bool remove(const T& value) {
		if (isEmpty()) {
			return false; // List is empty, nothing to remove
		}

		Node* current = head;
		Node* previous = (&Node::NIL);
		while (current != &Node::NIL) {
			if (current->value == value) {
				// Removing the first node
				if (current == head) {
					head = head->next;
					if (head == &Node::NIL) {
						last = &Node::NIL;
					}
				}
				else {
					previous->next = current->next;
					if (current == last) {
						last = previous;
					}
				}
				delete current;
				--count;
				return true;
			}
			previous = current;
			current = current->next;
		}
		return false; // Value not found
	}

	void popFront() {
		if (isEmpty()) {
			return; // List is empty, nothing to pop
		}

		Node* toDelete = head;
		head = head->next;
		delete toDelete;

		if (head == &Node::NIL) { // List became empty
			last = &Node::NIL;
		}
		--count;
	}

	Iterator begin() {
		return Iterator(head);
	}

	Iterator end() {
		return Iterator(&Node::NIL);
	}

	// Const versions for const iterators
	Iterator begin() const {
		return Iterator(head);
	}

	Iterator end() const {
		return Iterator(&Node::NIL);
	}

};

#endif /* SINGLYLINKEDLIST_H */