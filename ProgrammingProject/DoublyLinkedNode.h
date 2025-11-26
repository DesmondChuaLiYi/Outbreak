#ifndef DOUBLYLINKEDNODE_H
#define DOUBLYLINKEDNODE_H

template <class DataType>
class DoublyLinkedNode {
public:
	typedef DoublyLinkedNode<DataType> Node;
	static Node NIL;

	DataType value;
	Node* next;
	Node* previous;

	DoublyLinkedNode() {
		value = DataType();
		next = &NIL;
		previous = &NIL;
	}

	DoublyLinkedNode(const DataType& aValue) {
		value = aValue;
		next = &NIL;
		previous = &NIL;
	}

	// Add a node before the current one. (New Node is nearer the Head)
	void prepend(Node* newNode) {
		newNode->next = this;

		if (this->previous != &NIL) {
			newNode->previous = this->previous;
			this->previous->next = newNode;
		}
		this->previous = newNode;
	}

	// Add a node after the current one. (New Node is nearer the Tail)
	void append(Node* newNode) {
		newNode->next = this->next;
		newNode->previous = this;  // Missing line!
		if (this->next != &NIL) {
			this->next->previous = newNode;
		}
		this->next = newNode;
	}

	// Delete the current node from the list.
	void remove() {
		if (this->previous != &NIL) {
			this->previous->next = this->next;
		}
		if (this->next != &NIL) {
			this->next->previous = this->previous;
		}
		delete this;
	}
};

template <class DataType>
DoublyLinkedNode<DataType> DoublyLinkedNode<DataType>::NIL;

#endif /* DOUBLYLINKEDNODE_H */