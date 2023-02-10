/*                    COP 3502C Assignment 3
             This program is written by: Lawton Pittenger 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "leak_detector_c.h"
#define MAXLEN 30
FILE* fpOut;


//Struct declarations
typedef struct itemNode
{
	char name[MAXLEN];
	int count;
	struct itemNode *left, *right;
} itemNode;

typedef struct treeNameNode
{
	char treeName[MAXLEN];
	struct treeNameNode *left, *right;
	itemNode* theTree;
} treeNameNode;


//Function declarations
treeNameNode* createTreeNameNode(char* name);

treeNameNode* buildNameTree(char** names, int numNames);

void traverse_in_traverse(treeNameNode* root);

treeNameNode* searchNameNode(treeNameNode* root, char treeName[50]);

void insertItem(treeNameNode* treeRoot, char* itemToAdd, int count);

void inOrderItems(itemNode* root);

void inOrderNames(treeNameNode* root);

//Command functions
int item_before(itemNode* root, char* item);

int search(itemNode* root, char* item);

int get_count(itemNode* root);

void reduce(itemNode* root, char* item, int minusCount);

void delete_name(treeNameNode** root, char* name);

//Other helper functions
itemNode* deleteNameNode(treeNameNode* root, char* name);

itemNode* deleteNode(itemNode* root, char* item);

int findDepth(itemNode* root, int* leftDepth, int* rightDepth);

void findDepthRecursion(itemNode* root, int* depth, int* max);

void item_beforeRecursive(itemNode* root, char* item, int* count,int *foundItem);

void searchRecursive(itemNode* root, char* item, int* count);

void deleteAllTrees(treeNameNode** root);

void inOrderDeleteSubtrees(treeNameNode* root);


int main()
{
  atexit(report_mem_leak); //leak detector 
	int i = 0;
	int numNames, numItems, numOperations;
	FILE* fp = fopen("in.txt", "r");
	fpOut = fopen("out.txt", "w");
	char name[30], item[30], operation[16];
	int count;
	treeNameNode *tnn, *root;
	char** names;

	//Input The first line
	fscanf(fp, "%d %d %d", &numNames, &numItems, &numOperations);

	//Allocate space for the names that will be input
	names = (char**)malloc(numNames * sizeof(char*));
	//Input each name for the name tree
	for (i = 0; i < numNames; i++) {
		names[i] = (char*)malloc(30 * sizeof(char));
		fscanf(fp, "%s", name);
		memcpy(names[i], name, sizeof(name));		
	}

	// Create the name tree with the names supplied in the array names
	root = buildNameTree(names, numNames);
	tnn = root;

	//Get the items from the file and insert them into the appropriate tree
	for (i = 0; i < numItems; i++) {
		fscanf(fp, "%s %s %d", name, item, &count);
		tnn = searchNameNode(root, name);
		insertItem(tnn, item, count);
	}

	// Printout the tree, this will be saved to the out file
	traverse_in_traverse(root);

	int cnt = 100;
	int leftDepth=0, rightDepth=0, rslt;
	
 // Code for handling commands from the file
 // search for a particular item in a given tree and display the count of the item if it is found.
	for (i = 0; i < numOperations; i++) {
		fscanf(fp, "%s", operation);
		if (strcmp(operation, "search") == 0) {
			fscanf(fp, "%s %s", name, item);
			tnn = searchNameNode(root, name);
			if (tnn == NULL) {
				fprintf(fpOut,"%s does not exist\n", name);
			}
			else {
				cnt = search(tnn->theTree, item);
				if (cnt == -1) {
					fprintf(fpOut,"%s not found in %s\n",item,name);
				}
				else {
					fprintf(fpOut,"%d %s found in %s\n", cnt, item, name);
				}
			}
		}
    // this  command counts the  items  in  a  given  tree  coming  before  a  given item name (alphabetically).
		else if (strcmp(operation, "item_before") == 0) {
			fscanf(fp, "%s %s", name, item);
			tnn = searchNameNode(root,name);
			cnt = item_before(tnn->theTree, item);			
			fprintf(fpOut,"item before %s: %d\n", item, cnt);
		}
    // finds whether a given tree is height balanced or not
		else if (strcmp(operation, "height_balance") == 0) {
			fscanf(fp, "%s", name);
			tnn = searchNameNode(root, name);
			rslt = findDepth(tnn->theTree, &leftDepth, &rightDepth);
			if (rslt > 1) {
				fprintf(fpOut,"%s: left height %d, right height %d, difference %d, not balanced\n",name,leftDepth,rightDepth,rslt);
			}
			else {
				fprintf(fpOut,"%s: left height %d, right height %d, difference %d, balanced\n", name, leftDepth, rightDepth, rslt);
			}
		}
    // this command prints the total number of items in a given tree
		else if (strcmp(operation, "count") == 0) {
			fscanf(fp, "%s", name);
			tnn = searchNameNode(root, name);
			cnt = get_count(tnn->theTree);
			fprintf(fpOut,"%s count %d\n", name, cnt);
		}
    // this command deletes an item from a given tree
		else if (strcmp(operation, "delete") ==0 ) {
			fscanf(fp, "%s %s", name, item);
			tnn = searchNameNode(root, name);
			deleteNode(tnn->theTree, item);
			fprintf(fpOut,"%s deleted from %s\n", item, name);
		}
    // EXTRA CREDIT Function
    // this  command  reduces  the  count  of  an  item  in  a  given  tree.
		else if (strcmp(operation, "reduce") == 0) {
			fscanf(fp, "%s %s %d", name, item, &count);
			tnn = searchNameNode(root, name);
			reduce(tnn->theTree, item, count);
			fprintf(fpOut,"%s reduced\n", item);
		}
    // this command delte the entire tree of a given name.
		else if (strcmp(operation, "delete_name") == 0) {
			fscanf(fp, "%s", name);			
			delete_name(&root, name);	
			fprintf(fpOut,"%s deleted\n", name);
		}
	}

	// Free names generated at the top
	for (i = 0; i < numNames; i++) {
		free(names[i]);
	}
	free(names);
	// Free the memory for the trees
	deleteAllTrees(&root);

	fclose(fp);
	fclose(fpOut);
}

// Prints out the names of the name tree in order
void inOrderNames(treeNameNode* root)
{
	if (root == NULL) return;
	inOrderNames(root->left);
	fprintf(fpOut,"%s ", root->treeName);
	inOrderNames(root->right);
}

// This creates a new treeNameNode and returns it
treeNameNode* createTreeNameNode(char* name)
{
	treeNameNode* tnn = (treeNameNode*)malloc(sizeof(treeNameNode));
	tnn->left = NULL;
	tnn->right = NULL;
	tnn->theTree = NULL;
	memcpy(tnn->treeName, name, MAXLEN);
	return tnn;
}


// This inserts a new treeNameNode named "name" into
// the treeNameNode tree.  It places it in alphabetical order.
void insertName(treeNameNode** NameRoot, char* name)
{
	treeNameNode *tnn = createTreeNameNode(name);	
	treeNameNode* pCurrent, * pParent;
	
	// if tree is empty
	if (*NameRoot == NULL) {
		*NameRoot = tnn;
	}
	else {
		pCurrent = *NameRoot;
		pParent = NULL;

		while (1) {
			pParent = pCurrent;

			// go to the left of the tree, itemToAdd < pParent->name
			if (strcmp(name, pParent->treeName) < 0)
			{
				pCurrent = pCurrent->left;

				// Insert to the left
				if (pCurrent == NULL) {
					pParent->left = tnn;
					return;
				}
			} // go to the the right of the tree, itemToAdd >= pParent->name
			else {
				pCurrent = pCurrent->right;

				// insert to the right
				if (pCurrent == NULL) {
					pParent->right = tnn;
					return;
				}
			}
		}
	}
}

// This builds the Name Tree from the array of names
// passed to it.  numNames is the number of names in the array.
treeNameNode* buildNameTree(char** names,int numNames)
{
	treeNameNode *tnn=NULL, *prev=NULL;
	treeNameNode *root=NULL;
	int i = 0;
	for (i = 0; i < numNames; i++) {
		insertName(&tnn, names[i]);
	}	
	return tnn;
}

// This searches the treeNameNode tree for the treeNameNode node
// with the name "treeName".  It then returns that treeNameNode node.
treeNameNode* searchNameNode(treeNameNode* root, char treeName[50])
{
	treeNameNode* pCurrent, * pParent;

	// Tree is empty
	if (root == NULL) {
		return NULL;
	}
	else {
		pCurrent = root;
		pParent = NULL;

		while (1) {
			pParent = pCurrent;

			if (strcmp(treeName, pParent->treeName)==0) {
				return pParent;

			// go to the left of the tree, treeName < pParent->treeName
			}else if (strcmp(treeName, pParent->treeName) < 0)
			{
				pCurrent = pCurrent->left;

				// Not Found, return NULL
				if (pCurrent == NULL) {
					return pCurrent;					
				}
			} // got the the right of the tree, itemToAdd >= pParent->name
			else {
				pCurrent = pCurrent->right;

				// Not found, return NULL
				if (pCurrent == NULL) {					
					return NULL;
				}
			}
		}
	}
	return NULL;
}

// This inserts the item "itemToAdd" and its count at the correct
// position alphabetically.
void insertItem(treeNameNode* treeRoot, char* itemToAdd, int count)
{
	itemNode* item = (itemNode*)malloc(sizeof(itemNode));
	itemNode *pCurrent, *pParent;
	item->count = count;
	strcpy(item->name, itemToAdd);
	item->left = NULL;
	item->right = NULL;

	// if tree is empty
	if (treeRoot->theTree == NULL) {
		treeRoot->theTree = item;
	}
	else {
		pCurrent = treeRoot->theTree;
		pParent = NULL;

		while (1) {
			pParent = pCurrent;
			
			// go to the left of the tree, itemToAdd < pParent->name
			if (strcmp(itemToAdd, pParent->name) < 0)
			{
				pCurrent = pCurrent->left;

				// Insert to the left
				if (pCurrent == NULL) {
					pParent->left = item;
					return;
				}
			} // got the the right of the tree, itemToAdd >= pParent->name
			else {
				pCurrent = pCurrent->right;

				// insert to the right
				if (pCurrent == NULL) {
					pParent->right = item;
					return;
				}
			}
		}
	}
}

// This traverses an itemNode tree and prints the name of each element in order
void inOrderItems(itemNode* root)
{
	if (root == NULL) return;
	inOrderItems(root->left);
	fprintf(fpOut,"%s ", root->name);
	inOrderItems(root->right);
}

// This goes through the treeNameNode tree in order and prints out the names
// in turn it traverses each of the itemNode trees
void InOrderNameItems(treeNameNode* root)
{
	if (root == NULL) return;
	InOrderNameItems(root->left);
	fprintf(fpOut,"===%s===\n", root->treeName);
	inOrderItems(root->theTree);
	fprintf(fpOut,"\n");
	InOrderNameItems(root->right);
}

/*This traverses the treeNameNode tree and out
puts the name of each node.  It then traverses
the treeNameNode again and outputs the name of each
tree name, and then it ouputs all items from that category
*/ 
void traverse_in_traverse(treeNameNode* root)
{
	treeNameNode* pTreeNode;
	pTreeNode = root;
	
	inOrderNames(root);	
	fprintf(fpOut,"\n");
	InOrderNameItems(root);	
}

/* This recursive function determines the ordered position 
of the "item" searched for and passed the value back in *position.  
If item="", then it passes back the total number of items.  
You must initialize count to zero before calling the function.
foundItem is a flag that when set to 1 tells the function to stop counting
*/ 
void item_beforeRecursive(itemNode* root,char *item,int *position, int *foundItem)
{
	if (root == NULL) return;
	item_beforeRecursive(root->left,item,position,foundItem);

	if (*foundItem == 0) {
		(*position)++;
	}
	if (strcmp(root->name, item) == 0) {
		*foundItem = 1;
		return;
	}

	item_beforeRecursive(root->right,item, position,foundItem);
}

// Call this to get the count of the item before the passed in "item"
int item_before(itemNode* root, char* item)
{
	int count = 0;
	int foundItem = 0;
	item_beforeRecursive(root, item, &count,&foundItem);
	return count-1;
}

/* This recursive function determines the ordered position 
of the "item" searched for and passed the value back in *position.  
If item="", then it passes back the total number of items.  
You must initialize count to zero before calling the function.
 foundItem is a flag that when set to 1 tells the function to stop counting
*/
void item_CountRecursive(itemNode* root, char* item, int* count)
{
	if (root == NULL) return;
	item_CountRecursive(root->left, item, count);

	*count += root->count;

	item_CountRecursive(root->right, item, count);
}

// Return the total number of items in this item tree
int get_count(itemNode* root)
{
	int count = 0;	
	item_CountRecursive(root, "", &count);
	return count;
} 

/* Search for item in itemTree and return
the count for that item. If the item doesn't
exist return -1;
*/ 
int search(itemNode* root, char* item)
{
	int count = -1;
	searchRecursive(root, item, &count);
	return count;
}

/* Find the "item", pass back the "count" of the item if it is found.
if it is not found, pass back a -1 in "count";
For this function to work, set *count= -1 when calling the function
so that you can determine if a value was accessed.
*/ 
void searchRecursive(itemNode* root, char* item, int *count)
{
	if (root == NULL) return;
	searchRecursive(root->left, item, count);

	if (strcmp(root->name, item) == 0) {
		*count = root->count;
		return;
	}

	searchRecursive(root->right, item, count);
}

/* Find the "item", pass back the "count" of the item if it is found.
if it is not found, pass back a -1 in "count";
*count holds the amount to be subtracted from the item's count.
if the value of the item's count falls to zero or below, *count
is set to zero and passed back so that the itemNode can be deleted
*/ 
void reduceCountRecursive(itemNode* root, char* item, int *count)
{
	if (root == NULL) return;
	reduceCountRecursive(root->left, item, count);

	if (strcmp(root->name, item) == 0) {
		root->count -= *count;
		if (root->count <= 0) {
			*count = 0;
		}
		return;
	}

	reduceCountRecursive(root->right, item, count);
}

/* This reduces the count of the item "item" by "minusCount"
// It passes back a flag if the item's count is set to zero or less
*/
void reduce(itemNode* root, char* item,int minusCount)
{
	reduceCountRecursive(root, item, &minusCount);
	// Delete node of the count ended up at zero or less
	if (minusCount == 0) {
		deleteNode(root, item);
	}
}

/*  Finds the depth of the left and right subtrees
and passes the values back in *leftDepth and *rightDepth
returns returns the balance of the tree.
*/  
int findDepth(itemNode* root,int *leftDepth,int *rightDepth)
{
	int depth = 0;
	int maxLeft = 0, maxRight = 0;

	// If there is no left node, then there is
	// no substree, make maxLeft = -1 to reflect this
	if (root->left == NULL) {
		maxLeft = -1;
	}
	else {
		// Passes back the maximum depth of the left subtree in maxLeft;
		findDepthRecursion(root->left, &depth, &maxLeft);		
	}
	*leftDepth = maxLeft;

	if (root->right == NULL) {
		maxRight = -1;
	}
	else {
		// Passes back the maximum depth of the right subtree in maxRight;
		findDepthRecursion(root->right, &depth, &maxRight);	
	}
	*rightDepth = maxRight;

	//The absolute value of the depth of the left - right is the tree's balance
	return abs(*rightDepth - *leftDepth);
} 

//Find the depth of this subtree.  When first called, *depth
// and *max should be set to 0.
void findDepthRecursion(itemNode* root, int *depth,int *max)
{	
	if (root == NULL) return;

	if (root->left != NULL) {
		(*depth)++;
		if (*depth > *max)
			*max = *depth;
		findDepthRecursion(root->left, depth, max);
		(*depth)--;
	}

	if (root->right != NULL) {
		(*depth)++;
		if (*depth > * max)
			*max = *depth;
		findDepthRecursion(root->right, depth, max);
		(*depth)--;
	}
}

/* Given a non-empty binary search
   tree, return the node
   with minimum item value found in
   that tree.  */
itemNode* minValueNode(itemNode *node)
{
	itemNode *current = node;

	/* loop down to find the leftmost leaf */
	while (current && current->left != NULL)
		current = current->left;

	return current;
}

/* Given a binary search tree
   and a key, this function
   deletes the item and
   returns the new itemNode */
itemNode *deleteNode(itemNode* root, char* item)
{
	// base case
	if (root == NULL)
		return root;

	/* If the item to be deleted 
	is smaller than the root's
	item, then it lies in left subtree
   */
	if (strcmp(item,root->name) < 0) //  key < root->key)
		root->left = deleteNode(root->left, item);

	/* If the item to be deleted 
	is greater than the root's
	item, then it lies in right subtree 
  */
	else if (strcmp(item, root->name) > 0)
		root->right = deleteNode(root->right, item);

	/* if item is same as root's item, 
	then This is the node to be deleted 
  */
	else {
		// node with only one child or no child
		if (root->left == NULL) {
			itemNode *temp = root->right;
			free(root);
			return temp;
		}
		else if (root->right == NULL) {
			itemNode *temp = root->left;
			free(root);
			return temp;
		}

		/* node with two children: 
		Get the inorder successor (smallest in the right subtree)
    */
		itemNode *temp = minValueNode(root->right);

		// Copy the inorder 
		// successor's content to this node
		strcpy(root->name, temp->name);	
		root->count = temp->count;

		// Delete the inorder successor
		root->right = deleteNode(root->right, temp->name);
	}
	return root;
}

/*  This function traverses tree in post order to
	to delete each and every node of the tree */
void deleteTree(itemNode* root)
{
	if (root == NULL) return;

	/* first delete both subtrees */
	deleteTree(root->left);
	deleteTree(root->right);

	/* then delete the node */
	//fprintf(fpOut,"\n Deleting node: %d", node->data);
	//fprintf(fpOut,"deleting item %s\n", root->name);
	free(root);
}

/* Given a non-empty binary search
   tree, return the node
   with minimum item value found in
   that tree.  */
itemNode* minValueNameNode(treeNameNode* node)
{
	itemNode* current = node;

	/* loop down to find the leftmost leaf */
	while (current && current->left != NULL)
		current = current->left;

	return current;
}

// This deletes the treeNameNode tree with the name "name"
void delete_name(treeNameNode** root, char* name)
{
	treeNameNode *tnn = searchNameNode(*root, name);
	deleteTree(tnn->theTree);
	tnn->theTree = NULL;
	*root = deleteNameNode(*root, name);
}

/* Given a binary search tree
   and a key, this function
   deletes the item and
   returns the new itemNode */
itemNode* deleteNameNode(treeNameNode* root, char* name)
{
	// base case
	if (root == NULL)
		return root;

	/* If the item to be deleted 
	is smaller than the root's item, then it lies in left subtree
  */
	if (strcmp(name, root->treeName) < 0) //  key < root->key)
		root->left = deleteNode(root->left, name);

	/* If the item to be deleted 
	   is greater than the root's
	   item, then it lies in right subtree
  */
	else if (strcmp(name, root->treeName) > 0)
		root->right = deleteNode(root->right, name);

	/* if item is same as root's item, 
	   then This is the node
	   to be deleted
  */
	else {
		// node with only one child or no child
		if (root->left == NULL) {
			itemNode* temp = root->right;
			free(root);
			return temp;
		}
		else if (root->right == NULL) {
			itemNode* temp = root->left;
			free(root);
			return temp;
		}

		/* node with two children: 
		   Get the inorder successor
		  (smallest in the right subtree)
    */
		treeNameNode* temp = minValueNameNode(root->right);

		// Copy the inorder 
		// successor's content to this node
		strcpy(root->treeName, temp->treeName);
		root->theTree = temp->theTree;

		// Delete the inorder successor
		root->right = deleteNode(root->right, temp->treeName);
	}
	return root;
}

/*  This function traverses tree in post order to
	to delete each and every node of the tree */
void deleteNameTree(treeNameNode* root)
{
	treeNameNode* rt;

	//rt = *root;

	//rt->left;

	if (root == NULL) return;

	/* first delete both subtrees */
	deleteNameTree(root->left);
	deleteNameTree(root->right);

	/* then delete the node */
	//fprintf(fpOut,"Deleting the %s name tree\n", root->treeName);
	free(root);
	root = NULL;
}

// Delete the subtrees, fish, animal, etc.
void inOrderDeleteSubtrees(treeNameNode* root)
{
	if (root == NULL) return;
	inOrderDeleteSubtrees(root->left);
	//fprintf(fpOut,"Deleting the %s item tree\n", root->treeName);
	deleteTree(root->theTree);
	root->theTree = NULL;		
	inOrderDeleteSubtrees(root->right);
}

//Deletes the subtrees and the Name tree
void deleteAllTrees(treeNameNode** root)
{
	//fprintf(fpOut,"DELETING ALL TREES!\n");
	// Delete all of the subtree categories	
	inOrderDeleteSubtrees(*root);

	//inOrderNames(*root);
	//Delete the named trees
	deleteNameTree(*root);
}