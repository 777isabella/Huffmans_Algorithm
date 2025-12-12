/**************************************************************************
Class:		CSCI 3333.01 Data Structures and Algorithms
			Spring 2025
Project 5:	The Huffman's Algorithm
Name:		Isabella Morgan
ID:         20629238
Description: In this header file, we define nodeType and needed parts
			for Project 5.
**************************************************************************/
#ifndef HW5
#define HW5

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include <cstdlib>
#include <vector>
#include <stack>
#include <queue>

using namespace std;

//define Huffman tree node type
class nodeType
{
private:
	char ch;			//save a phrase with/without spaces
	int count;			//save frequency of the phrase
	string code;		//Huffman code for this node
	nodeType* left;		//left child pointer
	nodeType* right;    //right child pointer

public:
	bool operator<=(nodeType& p);
	bool operator<(nodeType& p);
	bool operator==(nodeType& p);
	void setNode(char x, int y, string c = "", nodeType* l = NULL, nodeType* r = NULL);
	nodeType& operator=(const nodeType& p);
	nodeType();
	nodeType(const nodeType& p);
	char getChar() { return ch; }
	int getCount() { return count; }
	string getCode() { return code; }
	void setCode(string s);
	nodeType* getLeft() { return left; }
	nodeType* getRight() { return right; }
	void clear(nodeType* ptr);
	~nodeType();
};
//define all the method nodeType class
//do the work

//implementation of nodeType methods
inline nodeType::nodeType() {
	ch = '\0';
	count = 0;
	code = "";
	left = NULL;
	right = NULL;
}

inline nodeType::nodeType(const nodeType& p) {
	ch = p.ch;
	count = p.count;
	code = p.code;
	left = p.left;
	right = p.right;
}

inline void nodeType::setNode(char x, int y, string c, nodeType* l, nodeType* r) {
	ch = x;
	count = y;
	code = c;
	left = l;
	right = r;
}

inline nodeType& nodeType::operator=(const nodeType& p) {
	ch = p.ch;
	count = p.count;
	code = p.code;
	left = p.left;
	right = p.right;
	return *this;
}

inline bool nodeType::operator<=(nodeType& p) {
	return count <= p.count;
}

inline bool nodeType::operator<(nodeType& p) {
	return count < p.count;
}

inline bool nodeType::operator==(nodeType& p) {
	return count == p.count;
}

inline void nodeType::setCode(string s) {
	code = s;
}

inline void nodeType::clear(nodeType* ptr) {
	if (ptr != NULL) {
		clear(ptr->left);
		clear(ptr->right);
		delete ptr;
	}
}

inline nodeType::~nodeType() {
	//only clear if this is the root managing the tree
	//individual nodes shouldn't delete their children
}

//helper function to insert node in sorted order
inline void insertSorted(vector<nodeType*>& list, nodeType* node) {
	vector<nodeType*>::iterator it = list.begin();
	while (it != list.end() && (*it)->getCount() < node->getCount()) {
		++it;
	}
	list.insert(it, node);
}

//helper function to generate Huffman codes by traversing tree
inline void generateCodes(nodeType* root, string code, map<char, string>& charCodes) {
	if (root == NULL) return;

	//if it's a leaf node (has a character)
	if (root->getLeft() == NULL && root->getRight() == NULL) {
		root->setCode(code);
		charCodes[root->getChar()] = code;
		return;
	}

	//traverse left with '0' and right with '1'
	if (root->getLeft() != NULL) {
		generateCodes(root->getLeft(), code + "0", charCodes);
	}
	if (root->getRight() != NULL) {
		generateCodes(root->getRight(), code + "1", charCodes);
	}
}

//helper function to decode a binnary string using Huffman tree
inline char decodeChar(nodeType* root, const string& encoded, int& pos) {
	nodeType* current = root;

	while (current->getLeft() != NULL || current->getRight() != NULL) {
		if (pos >= encoded.length()) {
			//error: ran out of bits
			return '\0'; 
		}

		if (encoded[pos] == '0') {
			current = current->getLeft();
		}
		else {
			current = current->getRight();
		}
		pos++;
	}

	return current->getChar();
}

#endif