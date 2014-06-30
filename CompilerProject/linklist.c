/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#include "linklist.h"
#include "compiler.h"
#include "stdafx.h"


LinkList linkList_createNew(int(*comparator) (void*, void*))
{
	LinkList ll;
	ll.head = NULL;
	ll.tail = NULL;
	ll.comparator = comparator;
	ll.size = 0;
	return ll;
}

LinkListNode* linkList_find(LinkList ll, void* value)
{
	LinkListNode* itr;
	itr = ll.head;
	for (;itr!=NULL;itr=itr->next)
	{
		if (ll.comparator(itr->element, value)) return itr;
	}

	return NULL;
}

LinkList linkList_remove(LinkList ll, void* value)
{
	if (ll.size == 0) return ll;
	if (ll.size == 1 && ll.comparator(value, ll.head->element))
	{
		LinkListNode* temp = ll.head;
		ll.head = NULL;
		ll.tail = NULL;
		if (temp->wasAddedByValue) free(temp->element);
		free(temp);
		ll.size--;
		return ll;
	}
	else if (ll.comparator(value, ll.head->element))
	{
		LinkListNode* temp = ll.head;
		ll.head = ll.head->next;
		if (temp->wasAddedByValue) free(temp->element);
		free(temp);
		ll.size--;
		return ll;
	}
	else
	{
		LinkListNode* prev = ll.head;
		LinkListNode* current = ll.head->next;

		while (current!=NULL)
		{
			if (ll.comparator(current->element, value))
			{
				if (ll.tail == current)
				{
					ll.tail = prev;
				}
				prev->next = current->next;
				if (current->wasAddedByValue) free(current->element);
				free(current);
				ll.size--;
				current = prev->next;
			}
			else
			{
				prev = current;
				current = current->next;
			}
		}
		return ll;
	}
}

LinkList linkList_add(LinkList ll, void* value)
{
	LinkListNode* newNode = linkList_createNewNode(value);
	newNode->next = ll.head;
	ll.head = newNode;
	ll.size++;
	if (ll.size == 1)
	{
		ll.tail = ll.head;
	}
	return ll;
}

LinkList linkList_addByValue(LinkList ll, void* value, int sizeOfElement)
{
	LinkListNode* newNode;
	void* newValue = malloc(sizeOfElement);
	memcpy(newValue, value, sizeOfElement);
	newNode = linkList_createNewNode(newValue);
	newNode->next = ll.head;
	newNode->wasAddedByValue = TRUE;
	newNode->sizeOfElement = sizeOfElement;
	ll.head = newNode;
	ll.size++;
	if (ll.size == 1)
	{
		ll.tail = ll.head;
	}
	return ll;
}

LinkList linkList_copy(LinkList ll)
{
	LinkList newList = linkList_createNew(ll.comparator);
	LinkListNode* temp;
	for (temp = ll.head; temp != NULL; temp = temp->next)
	{
		if (temp->wasAddedByValue) newList = linkList_addToTailByValue(newList, temp->element, temp->sizeOfElement);
		else newList = linkList_addToTail(newList, temp->element);
	}

	return newList;
}

LinkListNode* linkList_createNewNode(void* value)
{
	LinkListNode* newNode = (LinkListNode*)malloc(sizeof(LinkListNode));
	newNode->element = value;
	newNode->next = NULL;
	newNode->sizeOfElement = 0;
	newNode->wasAddedByValue = FALSE;
	return newNode;
}

LinkList linkList_clear(LinkList ll)
{
	while (ll.size>0)
	{
		ll = linkList_remove(ll, ll.head->element);
	}
	return ll;
}

LinkList linkList_addToTail(LinkList ll, void* value)
{
	LinkListNode* newNode = linkList_createNewNode(value);
	if (ll.size == 0)
	{
		ll.head = ll.tail = newNode;
		ll.size++;
		return ll;
	}
	else
	{
		ll.tail->next = newNode;
		ll.tail = newNode;
		ll.size++;
		return ll;
	}
}

LinkList linkList_addToTailByValue(LinkList ll, void* value, int sizeOfElement)
{
	LinkListNode* newNode;
	void* newValue = malloc(sizeOfElement);
	memcpy(newValue, value, sizeOfElement);
	newNode = linkList_createNewNode(newValue);
	newNode->wasAddedByValue = TRUE;
	newNode->sizeOfElement = sizeOfElement;
	if (ll.size == 0)
	{
		ll.head = ll.tail = newNode;
		ll.size++;
		return ll;
	}
	else
	{
		ll.tail->next = newNode;
		ll.tail = newNode;
		ll.size++;
		return ll;
	}
}

Stack stack_push(Stack s, void* value)
{
	return linkList_add(s, value);
}

Stack stack_pushByValue(Stack s, void* value, int sizeOfElement)
{
	return linkList_addByValue(s, value, sizeOfElement);
}

Stack stack_pop(Stack s)
{
	return linkList_remove(s, s.head->element);
}

void* stack_top(Stack s)
{
	if (s.head == NULL) return NULL;
	return s.head->element;
}

LinkList linkList_merge(LinkList l1, LinkList l2)
{
	if (l1.tail != NULL)
	{
		l1.tail->next = l2.head;
	}
	else
	{
		return l2;
	}
	l1.size += l2.size;
	return l1;
}
