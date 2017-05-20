#include <stdio.h>
#include "mutator.h"

using namespace std; 

class Network{
	public:
		int connectivity;
		vector<BPatch_basicBlock*>* blocks;
		
		Network(vector<BPatch_basicBlock*>* blocks, int connectivity);
		std::vector<int> getChildren(int parent);
		int getChildIndex(int parent, int childNumber);	
		void buildNetwork(BPatch_image* image, BPatch_binaryEdit* appBinary);
		void processChild(int myIndex, BPatch_basicBlock* parent, BPatch_image* image, BPatch_binaryEdit* appBinary);
		void printNode(int i, unsigned long s, unsigned long e, unsigned long h, unsigned long ps, unsigned long pe);
};

// Constructor - store vector of basic blocks and connectivity level
Network::Network(vector<BPatch_basicBlock*>* blocks, int connectivity){
	// Not valid network if... - size < 1 + connectivity, OR....?
	if (blocks->size() < 1+connectivity){
		// No possible network
		throw std::invalid_argument("Connectivity level must be less than the number of blocks");
	}
	
	this->connectivity = connectivity;
	this->blocks = blocks;
	
	cout << "Network is being created with connectivity: " << this->connectivity << endl;
	cout << "Blocks size: " << this->blocks->size() << endl;
}

// Returns the integer index of the 'childNumber'-th child of the item at index 'parent'
int Network::getChildIndex(int parent, int childNumber){
	// c-th child of index i is k * i + c where k=[1...k] connectivity level
	
	if(childNumber < 1 or childNumber > this->connectivity){
		return -1;  // Illegal child number
	}
	
	int childIndex = (this->connectivity * parent) + childNumber;	
	return childIndex;
}

// Returns a vector of the integer addresses of all children of a parent node at index 'parent'
std::vector<int> Network::getChildren(int parent){
	vector<int> children;
	for (int i = 1; i <= this->connectivity; i++){
		children.push_back(getChildIndex(parent, i));
	}
	return children;
}

// Generates the checker network and inserts snippets into the given image
void Network::buildNetwork(BPatch_image* image, BPatch_binaryEdit* appBinary){
	// Start at root: index 0
	int current_index = 0;
	BPatch_basicBlock* root = this->blocks->at(0);	
	
	// For all children, insert checker for parent, get start, end, hash
	for (int i : getChildren(current_index)){
		processChild(i, root, image, appBinary);
	}	
	delete root;
}

// Inserts a snippet into the node at myIndex which protects the parent node, and recursively processes myIndex's children
void Network::processChild(int myIndex, BPatch_basicBlock* parent, BPatch_image* image, BPatch_binaryEdit* appBinary){
	
	// Insert snippet protecting my parent at my entry point
	BPatchSnippetHandle* res = insert_checker(image, appBinary, parent, (this->blocks->at(myIndex)->findEntryPoint())); 
	if(res == NULL) {
	   fprintf(stderr, "Something wrong with inserting snippet\n");
	   exit(1);
	}

	// Process my children
	// For all children, insert checker for parent, get start, end, hash
	for (int i : getChildren(myIndex)){
		if (i < this->blocks->size()){
			processChild(i, this->blocks->at(myIndex), image, appBinary);
			// processChild(i, this->blocks->at(i), image, appBinary);
		} else {
			// If my child is outside the array bounds, it means I don't have a child here and need a dummy checker
			// createLeafChecker();
		}		
	}
	delete res;
}

void Network::printNode(int i, unsigned long s, unsigned long e, unsigned long h, unsigned long ps, unsigned long pe){
	cout << "Index:"<< i << " Start/end:" << s << "-" << e << " hash:" << h << " Protecting:" << ps << "-" << pe << endl;
}

