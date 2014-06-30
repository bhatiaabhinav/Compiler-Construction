/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _SET_
#define _SET_

#include "HashTable.h"

typedef struct
{
	int bucketIndex;
	LinkListNode* bucketNode;
} HashSetIterator;

typedef struct
{
	HashTable hashTable;
	HashSetIterator lastIterator; //Points to the last (largest hash table index and rightmost in link list) link list node.
} HashSet;

HashSet set_create(int maxSize, int(*comparator) (void*, void*), int(*hashFunction) (void* value, int hashTableSize));
HashSet set_add(HashSet, void*);
HashSet set_remove(HashSet, void*);
HashSet set_union(HashSet, HashSet);
int set_contains(HashSet, void*);
void* set_getElement(HashSet set, void* key);
HashSetIterator set_getIterator(HashSet set);
HashSetIterator set_getNextIterator(HashSet set, HashSetIterator currentIterator);
int set_iterationEnded(HashSet set, HashSetIterator iterator);
HashSet set_clear(HashSet set);

#endif
