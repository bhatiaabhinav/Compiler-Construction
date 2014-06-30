/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#include "set.h"
#include<stdlib.h>


HashSet set_add(HashSet mySet, void* value)
{
	int newBucket;
	if (set_contains(mySet, value)) return mySet;

	mySet.hashTable = hashTable_add(mySet.hashTable, value);
	newBucket = hashTable_find(mySet.hashTable, value);
	if (newBucket > mySet.lastIterator.bucketIndex)
	{
		mySet.lastIterator.bucketIndex = newBucket;
		mySet.lastIterator.bucketNode = mySet.hashTable.table[newBucket].head;
	}
	return mySet;
}

HashSet set_union(HashSet s1, HashSet s2)
{
	HashSet newSet = set_create(s1.hashTable.hashTableSize, s1.hashTable.comparator, s1.hashTable.hashFunction);

	HashSetIterator itr = set_getIterator(s1);

	for (; !set_iterationEnded(s1, itr); itr = set_getNextIterator(s1, itr))
	{
		newSet=set_add(newSet, itr.bucketNode->element);
	}

	itr = set_getIterator(s2);
	for (; !set_iterationEnded(s2, itr); itr = set_getNextIterator(s2, itr))
	{
		if (set_contains(s1, itr.bucketNode->element))
		{
			continue;
		}
		else
		{
			newSet = set_add(newSet, itr.bucketNode->element);
		}
	}

	return newSet;

}

HashSet set_remove(HashSet mySet,void* value)
{
	LinkListNode* yo;
	int bucketIndex = hashTable_find(mySet.hashTable, value);
	if (bucketIndex < 0) return mySet;

	yo = linkList_find(mySet.hashTable.table[bucketIndex], value);

	if (mySet.lastIterator.bucketIndex == bucketIndex && yo->next == NULL)
	{
		HashSetIterator prevItr = { -1, NULL };
		HashSetIterator curItr = set_getIterator(mySet);

		while (!set_iterationEnded(mySet, curItr))
		{
			prevItr = curItr;
			curItr = set_getNextIterator(mySet, curItr);
		}

		mySet.lastIterator = prevItr;
	}
	mySet.hashTable = hashTable_remove(mySet.hashTable, value);
	return mySet;
}


HashSetIterator set_getIterator(HashSet set)
{
	HashSetIterator it;
	int i;
	it.bucketNode = set.hashTable.table[0].head;
	it.bucketIndex = 0;

	
	for (i = 0; i < set.hashTable.hashTableSize; i++)
	{
		if (set.hashTable.table[i].size != 0)
		{
			it.bucketIndex = i;
			it.bucketNode = set.hashTable.table[i].head;
			return it;
		}
	}

	it.bucketIndex = -1;
	it.bucketNode = NULL;

	return it;
}

HashSetIterator set_getNextIterator(HashSet set, HashSetIterator currentIterator)
{
	HashSetIterator it;
	HashSetIterator errorIterator = { -1, NULL };
	if (currentIterator.bucketNode->next == NULL)
	{
		if (currentIterator.bucketIndex == set.hashTable.hashTableSize - 1)
		{
			return errorIterator;
		}
		else
		{
			int j;
			for (j = currentIterator.bucketIndex + 1; j < set.hashTable.hashTableSize; j++)
			{
				if (set.hashTable.table[j].size>0)
				{
					it.bucketIndex = j;
					it.bucketNode = set.hashTable.table[j].head;
					return it;
				}
			}
			return errorIterator;
		}
	}
	else
	{
		it.bucketNode = currentIterator.bucketNode->next;
		it.bucketIndex = currentIterator.bucketIndex;
		return it;
	}
}

int set_iterationEnded(HashSet set, HashSetIterator iterator)
{
	return iterator.bucketIndex < 0;
}

HashSet set_create(int maxSize, int(*comparator) (void*, void*), int(*hashFunction) (void* value, int hashTableSize))
{
	HashSet set;
	set.hashTable = hashTable_createNew(maxSize, comparator, hashFunction);
	set.lastIterator.bucketIndex = -1;
	set.lastIterator.bucketNode = NULL;
	return set;
}

int set_contains(HashSet set, void* value)
{
	return hashTable_find(set.hashTable, value) >= 0;
}

void* set_getElement(HashSet set, void* key)
{
	int index = hashTable_find(set.hashTable, key);
	if (index < 0) return NULL;
	return linkList_find(set.hashTable.table[index], key)->element;
}

HashSet set_clear(HashSet set)
{
	set.hashTable = hashTable_clear(set.hashTable);
	set.lastIterator.bucketIndex = -1;
	set.lastIterator.bucketNode = NULL;
	return set;
}


