#include <stdio.h>

using namespace std; 

class BBTreeNode{
	public:
		int order;							// Store the order of the tree (max # of children)
		unsigned long start;				// Store the start address of this block
		unsigned long end;					// Store the end address of this block
		unsigned long hash;					// Store the hash value of this block
		BBTreeNode(unsigned long start, unsigned long end);						// Constructor
		// void addChild(BBTreeNode child);
		std::vector<BBTreeNode*> children;	// Child nodes 
};

BBTreeNode::BBTreeNode(unsigned long start, unsigned long end){
	// cout << "TreeNode is being created" << endl;
	this->start = start;
	this->end = end;
}

// void BBTreeNode::addChild(BBTreeNode child){
// }




class BBTree{	
	public:
		int order;
		BBTreeNode* root;
		BBTree(int o);
		void add(unsigned long start, unsigned long end);
		BBTreeNode* lastParent;		// The parent of the last node inserted. Try to insert here first. If full, insert at leftmost.
		BBTreeNode* leftmostChild;	// Leftmost leaf node. When lastParent has order children, this node gets the next insert
};

BBTree::BBTree(int o){
	// Will need to check if order is possible with number of basic blocks. TODO
	order = o;
	root = nullptr;
	leftmostChild = nullptr;
	cout << "Tree is being created with order: " << order << endl;
}

void BBTree::add(unsigned long start, unsigned long end){
	// cout << "Adding: <" << start << ", " << end << ">" << endl;
	
	// BBTreeNode newNode(start, end);
	
	// Insert at root if no root
	if (!root){
		// cout << "New root ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
		// root = &newNode;
		root = new BBTreeNode(start, end);
		lastParent = root;

	} else {
		// Otherwise 
		// cout << "Current root: " << root->start << endl;
		cout << "Current lastParent: " << (*lastParent).start << endl;
		
		// Try to insert at lastParent
		if (lastParent->children.size() < order){
			cout << "Inserting at lParent" << endl; // lastParent->children.size() << endl;

			lastParent->children.push_back(new BBTreeNode(start, end));
			// lastParent->children.push_back(&newNode);
			
			cout << "Inserted at lParent. Size: " << lastParent->children.size() << endl;
			
			if (!leftmostChild){
				// If this is the first insert, set leftmostChild
				leftmostChild = lastParent->children.back();

			}
			cout << "Current lmChild: " << leftmostChild->start << endl;
			
		} else{
			// If lastParent full, insert at leftmostChild and set new node to be leftmostChild.
			cout << "lParent full. Inserting at lmChild" << endl;
			
			// lastParent = leftmost
			lastParent = leftmostChild;
			
			// insert child
			lastParent->children.push_back(new BBTreeNode(start, end));
			
			// leftmost = child
			leftmostChild = lastParent->children.back();
		}
		
	}
	
}


