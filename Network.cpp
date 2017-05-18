#include <stdio.h>
#include "mutator.h"

using namespace std; 

class Network{
	public:
		int connectivity;
		vector<BPatch_basicBlock*>* blocks;
		
		Network(vector<BPatch_basicBlock*>* blocks, int connectivity);		
		int getChildIndex(int parent, int childNumber);
		std::vector<int> getChildren(int parent);
		void buildNetwork(BPatch_image* image, BPatch_binaryEdit* appBinary);
		void processChild(int myIndex, BPatch_basicBlock* parent, BPatch_image* image, BPatch_binaryEdit* appBinary);
		void printNode(int i, unsigned long s, unsigned long e, unsigned long h, unsigned long ps, unsigned long pe);
		
		// vector<BPatch_function*>* functions;
		// Network(vector<BPatch_function*>* functions, int connectivity);
};

// Network::Network(vector<BPatch_function*>* blocks, int connectivity){
Network::Network(vector<BPatch_basicBlock*>* blocks, int connectivity){
	// Not valid network if... - size < 1 + connectivity, OR....?
	if (blocks->size() < 1+connectivity){
		// invalid. return null or?
	}
	
	this->connectivity = connectivity;
	this->blocks = blocks;
	
	cout << "Network is being created with connectivity: " << this->connectivity << endl;
	cout << "Blocks size: " << this->blocks->size() << endl;

	
	
	
	

	
	
	
	
	
}

int Network::getChildIndex(int parent, int childNumber){
	// c-th child of index i is k * i + c where k=[1...k] connectivity level
	
	if(childNumber < 1 or childNumber > this->connectivity){
		return -1;  // Illegal child number
	}
	
	int childIndex = (this->connectivity * parent) + childNumber;
	
	// Check vector bounds before returning?
	
	return childIndex;
}

// Get all children ~
std::vector<int> Network::getChildren(int parent){
	vector<int> children;
	for (int i = 1; i <= this->connectivity; i++){
		children.push_back(getChildIndex(parent, i));
	}
	return children;
}

void Network::buildNetwork(BPatch_image* image, BPatch_binaryEdit* appBinary){
	// Start at root: index 0, get start, end, hash
	int current_index = 0;
	BPatch_basicBlock* root = this->blocks->at(0);
	// printNode(0, 
			// root->getStartAddress(), 
			// root->getEndAddress(), 
			// calc_hash_sum(root), 123456, 123456);
	
	
	// BPatchSnippetHandle* res = insert_checker(image, appBinary, root, (this->blocks->at(1)->findEntryPoint())); 
	//if(res == NULL) {
	//   fprintf(stderr, "Something wrong with inserting snippet\n");
	//   exit(1);
	// }
	
	
	// For all children, insert checker for parent, get start, end, hash
	for (int i : getChildren(current_index)){
		processChild(i, root, image, appBinary);
	}	
}

void Network::processChild(int myIndex, BPatch_basicBlock* parent, BPatch_image* image, BPatch_binaryEdit* appBinary){
	// cout << "processing child " << myIndex << endl;
	
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
			processChild(i, this->blocks->at(i), image, appBinary);
		} else {
			// If my child is outside the array bounds, add it to the unprotected list
			// exit(0);  // Should do something else
			// cout << "Out of array bounds" << endl;
		}		
	}
	// remove from blocks?
}

void Network::printNode(int i, unsigned long s, unsigned long e, unsigned long h, unsigned long ps, unsigned long pe){
	cout << "Index:"<< i << " Start/end:" << s << "-" << e << " hash:" << h << " Protecting:" << ps << "-" << pe << endl;
}



















