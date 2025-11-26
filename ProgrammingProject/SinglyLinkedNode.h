#ifndef SINGLYLINKEDNODE_H
#define SINGLYLINKEDNODE_H

template <class T>
class SinglyLinkedNode { // Singly linked node
public:
	typedef SinglyLinkedNode<T> Node;
	static Node NIL;

	T value;
	Node* next;
	
	// Constructor
	SinglyLinkedNode() : value(T()), next(&NIL) {}

	// Parameterised constructor
	SinglyLinkedNode(T val) : value(val), next(&NIL) {}
};

template <class T>
SinglyLinkedNode<T> SinglyLinkedNode<T>::NIL = SinglyLinkedNode<T>(T());

#endif /* NODE_H */