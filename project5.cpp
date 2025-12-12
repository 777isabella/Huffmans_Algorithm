/**************************************************************************
Class:			CSCI 3333.01 Data Structures and Algorithms
				Spring 2025
Project 5:		The Huffman's Algorithm
Name:			Isabella Morgan
ID:             20629238
Description: Implement the Huffman's algorithm to compress and uncompress text documents.
Input files: bible.txt
	This is the original document to encode (compress).
Output file: bible_encoded.txt, bible_decoded.txt
	bible_encoded.txt is the encoded file of the bible.txt
	bible_decoded.txt is the decoded file of bible_encoded.txt
   Note: bible_decoded.txt must be identical to bible.txt.
**************************************************************************/

#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <sstream>
#include <ctype.h> 
#include <utility>
#include <queue>
#include <time.h>

//define all the needed parts in this header file
# include "project5_head.h"

using namespace std;

int main()
{
	ifstream myfile;
	ofstream outfile;

	map<char, int> charCounts;	//hash maps to store all characters with frequencies
	map<char, int>::iterator it;
	map<char, string> charCodes; //map for char-Huffman code
	map<char, string>::iterator cit;
	vector<nodeType*> nodeList; //list of nodes
	vector<nodeType*>::iterator vit;
	queue<nodeType*> nodeQ;		//node ptr queue for bfs search
	char ch;
	nodeType* ptr, * lc, * rc;
	string str, EMPTY_STR = "";
	int i, count1, count2;

	time_t start, end;

	start = time(NULL);

	//------------------------------
	//Part 1: Count char frequencies
	cout << "loading input text ...... " << endl;
	myfile.open("bible.txt"); // input file
	charCounts.clear();

	while (!myfile.eof())
	{
		myfile.get(ch);
		if (!myfile.eof()) {
			charCounts[ch]++;
		}
	}
	myfile.close();
	cout << "finished counting frequencies of chars in the input text " << endl;
	cout << "Total unique characters: " << charCounts.size() << endl;

	//---------------------------------------------------------
	//Part 2: build tree node list and sort in increasing order
	it = charCounts.begin();
	while (it != charCounts.end())
	{
		ptr = new nodeType();
		ptr->setNode(it->first, it->second);
		insertSorted(nodeList, ptr);
		++it;
	}
	cout << "finished building tree node list" << endl;

	//----------------------------------------
	//Part 3: Build Huffman tree
	while (nodeList.size() > 1)
	{
		// Get two nodes with lowest frequencies
		lc = nodeList[0];
		rc = nodeList[1];

		// Remove them from the list
		nodeList.erase(nodeList.begin());
		nodeList.erase(nodeList.begin());

		// Create new parent node with combined frequency
		ptr = new nodeType();
		ptr->setNode('\0', lc->getCount() + rc->getCount(), "", lc, rc);

		// Insert back into sorted list
		insertSorted(nodeList, ptr);
	}
	cout << "finished building Huffman tree" << endl;

	// The root of the Huffman tree
	nodeType* root = nodeList[0];

	// Generate Huffman codes
	charCodes.clear();
	generateCodes(root, "", charCodes);

	cout << "Generated Huffman codes for " << charCodes.size() << " characters" << endl;

	//------------------------------------------
	//Part 4: Encode the input text bible.txt
	//and save the encoded file into bible_encoded.txt
	myfile.open("bible.txt");
	outfile.open("bible_encoded.txt");

	string encodedStr = "";
	while (!myfile.eof())
	{
		myfile.get(ch);
		if (!myfile.eof()) {
			encodedStr += charCodes[ch];
		}
	}

	outfile << encodedStr;
	myfile.close();
	outfile.close();

	cout << "finished encoding" << endl;
	cout << "Original size (chars): " << charCounts.size() << " unique" << endl;
	cout << "Encoded string length: " << encodedStr.length() << " bits (as string)" << endl;

	//-----------------------------------------
	//Part 5: Decode the encoded file bible_encoded.txt
	//and save the decoded file into bible_decoded.txt
	myfile.open("bible_encoded.txt");
	outfile.open("bible_decoded.txt");

	// Read the entire encoded string
	string encodedContent = "";
	getline(myfile, encodedContent);
	myfile.close();

	// Decode using the Huffman tree
	int pos = 0;
	while (pos < encodedContent.length()) {
		char decodedChar = decodeChar(root, encodedContent, pos);
		if (decodedChar != '\0') {
			outfile.put(decodedChar);
		}
	}

	outfile.close();
	cout << "finished decoding" << endl;

	//------------------------------------------------------------------
	//Part 6: Check whether bible.txt is the same as bible_decoded.txt
	ifstream origFile("bible.txt"), decodedFile("bible_decoded.txt");
	bool theSame = true;
	char ch1, ch2;

	while (!origFile.eof() && !decodedFile.eof()) {
		origFile.get(ch1);
		decodedFile.get(ch2);
		if (ch1 != ch2) {
			theSame = false;
			break;
		}
	}

	// Check if both files ended at the same time
	if (origFile.eof() && !decodedFile.eof() ||
		!origFile.eof() && decodedFile.eof()) {
		theSame = false;
	}

	origFile.close();
	decodedFile.close();

	if (theSame == true)
		cout << "Yes, bible_decoded.txt is the same as bible.txt." << endl;
	else
		cout << "No, bible_decoded.txt is not the same as bible.txt." << endl;

	//print the time used
	end = time(NULL);
	cout << endl << "+++++++++++++++++++++" << endl;
	cout << "time used: " << difftime(end, start) << " seconds" << endl;

	// Clean up
	root->clear(root);

	//The work is done
	return 0;
}