/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _LINKLIST_H_
#define _LINKLIST_H_
#define stack_createNew linkList_createNew
#include "Boolean.h"

struct _LinkListNode;
typedef struct _LinkListNode LinkListNode;

struct _LinkListNode
{
	int sizeOfElement;
	Boolean wasAddedByValue;
	void* element;
	LinkListNode* next;
};

//Pass it around by value
typedef struct
{
	int size;
	LinkListNode* head;
	LinkListNode* tail;
	int(*comparator) (void* value1, void* value2);
} LinkList;

typedef LinkList Stack;

LinkListNode* linkList_createNewNode(void* value);
LinkList linkList_createNew(int(*comparator) (void*, void*));
LinkListNode* linkList_find(LinkList ll, void* value);
LinkList linkList_remove(LinkList ll, void* value);
LinkList linkList_add(LinkList ll, void* value);
LinkList linkList_addByValue(LinkList ll, void* value, int sizeOfElement);
LinkList linkList_addToTail(LinkList ll, void* value);
LinkList linkList_addToTailByValue(LinkList ll, void* value, int sizeOfElement);
LinkList linkList_clear(LinkList ll);
LinkList linkList_copy(LinkList ll);

//currupts linklist l1
LinkList linkList_merge(LinkList l1, LinkList l2);

Stack stack_push(Stack s, void* value);
Stack stack_pushByValue(Stack s, void* value, int sizeOfElement);
Stack stack_pop(Stack s);
void* stack_top(Stack s);

#endif

