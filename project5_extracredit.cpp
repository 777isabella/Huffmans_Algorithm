/**************************************************************************
Class:			CSCI 3333.01 Data Structures and Algorithms
				Spring 2025
Project 5:		The Huffman's Algorithm extra credit bit-wise compression
Name:			Isabella Morgan
ID:             20629238
Description: Implement the Huffman's algorithm to compress and uncompress text documents.
Input files: bible.txt
	This is the original document to encode (compress).
Output file: bible_encoded.txt, bible_decoded.txt
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

# include "project5_head.h"

using namespace std;

//function to write bits to a file
void writeBits(ofstream& outfile, const string& bitString) {
	//1st, write the length of the bit string 
	// (so we know how many bits to read)
	unsigned long long bitLength = bitString.length();
	outfile.write(reinterpret_cast<const char*>(&bitLength), sizeof(bitLength));

	//convert bit string to bytes
	unsigned char byte = 0;
	int bitCount = 0;

	for (size_t i = 0; i < bitString.length(); i++) {
		byte = (byte << 1) | (bitString[i] - '0');
		bitCount++;

		if (bitCount == 8) {
			outfile.write(reinterpret_cast<const char*>(&byte), 1);
			byte = 0;
			bitCount = 0;
		}
	}

	//write remaining bits (padded with zeros)
	if (bitCount > 0) {
		//pad w zeroes
		byte = byte << (8 - bitCount); 
		outfile.write(reinterpret_cast<const char*>(&byte), 1);
	}
}

//function to read bits from a file
string readBits(ifstream& infile) {
	//read the length of the bit string
	unsigned long long bitLength;
	infile.read(reinterpret_cast<char*>(&bitLength), sizeof(bitLength));

	string bitString = "";
	unsigned char byte;
	unsigned long long bitsRead = 0;

	while (bitsRead < bitLength) {
		infile.read(reinterpret_cast<char*>(&byte), 1);

		//extract bits from bytes
		for (int i = 7; i >= 0 && bitsRead < bitLength; i--) {
			bitString += ((byte >> i) & 1) ? '1' : '0';
			bitsRead++;
		}
	}

	return bitString;
}

//func to save the Huffman tree structure (for decoding)
void saveTree(ofstream& outfile, nodeType* root) {
	if (root == NULL) {
		//marker for NULL
		outfile.put('0');
		return;
	}

	if (root->getLeft() == NULL && root->getRight() == NULL) {
		//leaf node
		outfile.put('1');
		outfile.put(root->getChar());
	}
	else {
		//internal node
		outfile.put('2');
		saveTree(outfile, root->getLeft());
		saveTree(outfile, root->getRight());
	}
}

//func to load the Huffman tree structure
nodeType* loadTree(ifstream& infile) {
	char marker;
	infile.get(marker);

	if (marker == '0') {
		return NULL;
	}
	else if (marker == '1') {
		//leaf node
		char ch;
		infile.get(ch);
		nodeType* node = new nodeType();
		node->setNode(ch, 0);
		return node;
	}
	else {
		//internal node
		nodeType* node = new nodeType();
		node->setNode('\0', 0);
		nodeType* left = loadTree(infile);
		nodeType* right = loadTree(infile);
		node->setNode('\0', 0, "", left, right);
		return node;
	}
}

int main()
{
	ifstream myfile; // Set up to read Huckleberry
	ofstream outfile;

	map<char, int> charCounts;//hash maps to store all phrases with frequencies
	map<char, int>::iterator it;
	map<char, string> charCodes; //map for char-Huffman code
	map<char, string>::iterator cit;
	vector<nodeType*> nodeList; //top 10 lists of phrase of length 1 tp 10
	vector<nodeType*>::iterator vit;
	/*queue<nodeType*> nodeQ;		*///node ptr queue for bsf search
	char ch;
	nodeType* ptr, * lc, * rc;
	//string str, EMPTY_STR = "";
	//int i, count1, count2;

	time_t start, end;
	start = time(NULL);

	//------------------------------
	//Part 1: Count char frequencies
	cout << "loading input text ...... " << endl;
	myfile.open("bible.txt");
	charCounts.clear();

	long long totalChars = 0;
	while (!myfile.eof())
	{
		//count and record frequencies of all letters/characters 
		//in the input file
		//......
		myfile.get(ch);
		if (!myfile.eof()) {
			charCounts[ch]++;
			totalChars++;
		}
	}
	myfile.close();
	cout << "finished counting frequencies of chars in the input text " << endl;
	cout << "Total characters: " << totalChars << endl;
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
		lc = nodeList[0];
		rc = nodeList[1];

		nodeList.erase(nodeList.begin());
		nodeList.erase(nodeList.begin());

		ptr = new nodeType();
		ptr->setNode('\0', lc->getCount() + rc->getCount(), "", lc, rc);

		insertSorted(nodeList, ptr);
	}
	cout << "finished building Huffman tree" << endl;

	nodeType* root = nodeList[0];

	// Generate Huffman codes
	charCodes.clear();
	generateCodes(root, "", charCodes);

	cout << "Generated Huffman codes for " << charCodes.size() << " characters" << endl;

	//------------------------------------------
	//Part 4: Encod the input text bible.txt
	//and save the encoded file into bible_encoded.txt
	//......
	//encode with bit-level compression
	myfile.open("bible.txt");
	outfile.open("bible_encoded.bin", ios::binary);

	// Save the tree structure first
	saveTree(outfile, root);

	// Build encoded bit string
	string encodedStr = "";
	while (!myfile.eof())
	{
		myfile.get(ch);
		if (!myfile.eof()) {
			encodedStr += charCodes[ch];
		}
	}
	myfile.close();

	// Write bits to file
	writeBits(outfile, encodedStr);
	outfile.close();

	cout << "finished encoding with bit-level compression" << endl;
	cout << "Original size: " << totalChars << " bytes" << endl;
	cout << "Encoded bit length: " << encodedStr.length() << " bits" << endl;
	cout << "Compressed size: ~" << (encodedStr.length() / 8) << " bytes" << endl;
	cout << "Compression ratio: " << fixed << setprecision(2)
		<< (100.0 * (encodedStr.length() / 8.0) / totalChars) << "%" << endl;

	//-----------------------------------------
	//Part 5: Decode the bit-compressed file
	myfile.open("bible_encoded.bin", ios::binary);
	outfile.open("bible_decoded.txt");

	// Load the tree structure
	nodeType* decodedRoot = loadTree(myfile);

	// Read the encoded bits
	string encodedContent = readBits(myfile);
	myfile.close();

	// Decode using the Huffman tree
	int pos = 0;
	while (pos < encodedContent.length()) {
		char decodedChar = decodeChar(decodedRoot, encodedContent, pos);
		if (decodedChar != '\0') {
			outfile.put(decodedChar);
		}
	}

	outfile.close();
	cout << "finished decoding" << endl;

	//------------------------------------------------------------------
	//Part 6: Check whether bible.txt is the same as bibile_decoded.txt
	//Note: You must include this part (or an equivalent part) 
	//in your program to verify whether the encoding 
	// and decoding processes are right.
	ifstream origFile("bible.txt"), decodedFile("bible_decoded.txt");
	bool theSame = true;
	char ch1, ch2;

	long long charCount = 0;
	while (!origFile.eof() && !decodedFile.eof()) {
		origFile.get(ch1);
		decodedFile.get(ch2);
		if (ch1 != ch2) {
			theSame = false;
			cout << "Difference found at character " << charCount << endl;
			break;
		}
		charCount++;
	}

	if (origFile.eof() && !decodedFile.eof() ||
		!origFile.eof() && decodedFile.eof()) {
		theSame = false;
	}

	origFile.close();
	decodedFile.close();

	if (theSame == true)
		cout << "\n*** YAYYYY! Yes, bible_decoded.txt is the same as bible.txt. ***" << endl;
	else
		cout << "\n*** No:(, bible_decoded.txt is NOT the same as bible.txt. ***" << endl;

	//get actual file sizes
	ifstream origSize("bible.txt", ios::binary | ios::ate);
	ifstream compSize("bible_encoded.bin", ios::binary | ios::ate);

	if (origSize && compSize) {
		long long origBytes = origSize.tellg();
		long long compBytes = compSize.tellg();
		cout << "\nActual file sizes:" << endl;
		cout << "  Original: " << origBytes << " bytes" << endl;
		cout << "  Compressed: " << compBytes << " bytes" << endl;
		cout << "  Compression ratio: " << fixed << setprecision(2)
			<< (100.0 * compBytes / origBytes) << "%" << endl;
		cout << "  Space saved: " << (origBytes - compBytes) << " bytes ("
			<< fixed << setprecision(2) << (100.0 * (origBytes - compBytes) / origBytes)
			<< "%)" << endl;
	}

	//print the time used
	end = time(NULL);
	cout << "\n+++++++++++++++++++++" << endl;
	cout << "time used: " << difftime(end, start) << " seconds" << endl;

	// Clean up
	root->clear(root);
	decodedRoot->clear(decodedRoot);

	return 0;
}