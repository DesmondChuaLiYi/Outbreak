#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <iostream>
#include <string>

template <class Key, class Value >
class HashTable {
private:
	struct Node {
		Key key;
		Value value;
		Node* next;

		// Sentinel node
		static Node NIL;

		// Constructor
		Node(const Key& k, const Value& v) : key(k), value(v), next(&Node::NIL) {}

		// Default constructor for NIL
		Node() : key(Key()), value(Value()), next(nullptr) {}
	};

	Node** table; // Array of pointers (buckets)
	int tableSize; // Size of the array
	int numElements; // Number of elements stored

	int hash(const Key& key) const {
		unsigned long hashValue = 5381;

		// Convert key to string representation for hashing
		std::string keyStr = keyToString(key);

		for (char c : keyStr) {
			hashValue = ((hashValue << 5) + hashValue) + c;
		}

		return hashValue % tableSize;
	}

	// Helper function to convert key to string
	std::string keyToString(const std::string& key) const {
		return key;
	}

	// Template specialization for other types
	template<typename T>
	std::string keyToString(const T& key) const {
		return std::to_string(key);
	}

public:
	// Constructor
	HashTable(int size = 50) : tableSize(size), numElements(0) {
		table = new Node * [size];

		// Initialise all buckets to NIL
		for (int i = 0; i < size; ++i) {
			table[i] = &Node::NIL;
		}
	}

	// Destructor
	~HashTable() {
		for (int i = 0; i < tableSize; ++i) {
			Node* current = table[i];

			while (current != &Node::NIL) {
				Node* toDelete = current;
				current = current->next;
				delete toDelete;
			}
		}
		delete[] table; // Delete the array
	}

	void insert(const Key& key, const Value& value) {
		int index = hash(key);

		// Check if the key already exists in the bucket
		Node* current = table[index];
		while (current != &Node::NIL) {
			if (current->key == key) {
				// Key exists, update value
				current->value = value;
				return;
			}
			current = current->next;
		}

		// Key does not exist, insert new node at the head of the bucket
		Node* newNode = new Node(key, value);
		newNode->next = table[index];
		table[index] = newNode;

		numElements++;
	}

	// Find value by key
	Value* search(const Key& key) const {
		int index = hash(key);

		// Search through the chain in the bucket
		Node* current = table[index];
		while (current != &Node::NIL) {
			if (current->key == key) {
				return const_cast<Value*>(&(current->value));
			}
			current = current->next;
		}

		return nullptr; // Key not found
	}

	void remove(const Key& key) {
		int index = hash(key);

		Node* current = table[index];
		Node* previous = &Node::NIL;

		// Search for the key
		while (current != &Node::NIL) {
			if (current->key == key) {
				if (previous == &Node::NIL) {
					table[index] = current->next; // Remove head
				}
				else {
					previous->next = current->next;
				}
				delete current;
				numElements--;
				return;
			}
			previous = current;
			current = current->next;
		}
		return; // Key not found
	}

	// Get number of elements in hash table
	int getSize() const {
		return numElements;
	}

	// Check if the hash table is empty
	bool isEmpty() const {
		return numElements == 0;
	}
};

template <class Key, class Value>
typename HashTable<Key, Value>::Node HashTable<Key, Value>::Node::NIL;

#endif /* HASHTABLE_H */