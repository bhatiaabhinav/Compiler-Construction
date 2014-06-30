/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _TREE_
#define  _TREE_
#define MAX_NO_CHILDREN 20

struct _TreeNode;

typedef struct _TreeNode TreeNode;

typedef TreeNode* TreeLink;

struct _TreeNode
{
	void* data;
	TreeLink children[MAX_NO_CHILDREN];
	TreeLink parent;
	int no_of_children;
	int childID;
};

typedef struct 
{
	TreeLink root;
	int totalNodes;
	int totalLeafNodes;
}Tree;


Tree tree_createNew();
Tree tree_add(Tree, TreeLink, void* value);
//Tree tree_remove(Tree, TreeLink, void* value);


#endif