/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#include "Tree.h"
#include <stdlib.h>


Tree tree_createNew()
{
	Tree t;
	t.root = NULL;
	t.totalLeafNodes = 0;
	t.totalNodes = 0;

	return t;
}

Tree tree_add(Tree t, TreeLink parent, void* value)
{
	TreeNode* newNode;

	if (parent == NULL && t.totalNodes != 0) return t;

	newNode = (TreeNode*)malloc(sizeof(TreeNode));
	newNode->data = value;
	newNode->no_of_children = 0;
	newNode->parent = parent;
	
	
	if (t.totalNodes > 0)
	{
		newNode->childID = parent->no_of_children;
		parent->children[parent->no_of_children] = newNode;
		parent->no_of_children++;
		
		if (parent->no_of_children > 1)
		{
			t.totalLeafNodes++;
		}
	}
	else
	{
		newNode->childID = 0;//or should it be -1
		t.root = newNode;
		t.totalLeafNodes = 1;
	}

	t.totalNodes++;
	

	return t;
}


