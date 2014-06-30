/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#include "HashTable.h"
#include "parserDef.h"
#include <stdlib.h>

HashTable hashTable_createNew(int hashTableSize, int(*comparator) (void*, void*), int(*hashFunction) (void* value, int hashTableSize))
{
	HashTable ht = { 0 };
	ht.size = 0;
	ht.comparator = comparator;
	ht.hashFunction = hashFunction;
	ht.hashTableSize = hashTableSize;
	ht.table = (LinkList*)malloc(hashTableSize*sizeof(LinkList));

	int i;
	for (i = 0; i < hashTableSize; i++)
	{
		ht.table[i] = linkList_createNew(comparator);
	}
	return ht;
}

int hashTable_find(HashTable ht, void* value)
{
	int hash = ht.hashFunction(value, ht.hashTableSize);
	if (linkList_find(ht.table[hash], value)) return hash;
	return -1;
}

//int symbolTable_find(HashTable ht, Symbol s)
//{	
//	
//	int hash = ht.hashFunction(s.symbolName, ht.hashTableSize);
//	if (linkList_find(ht.table[hash], &s)) return hash;
//	return -1;
//}

HashTable hashTable_remove(HashTable ht, void* value)
{
	int i = hashTable_find(ht, value);
	if (i < 0) return ht;
	ht.table[i] = linkList_remove(ht.table[i], value);
	ht.size--;
	return ht;
}

HashTable hashTable_add(HashTable ht, void* value)
{
	int hash = ht.hashFunction(value, ht.hashTableSize);
	ht.table[hash] = linkList_add(ht.table[hash], value);
	ht.size++;
	return ht;
}

HashTable hashTable_clear(HashTable ht)
{
	int i;
	for (i = 0; i < ht.hashTableSize; i++)
	{
		int reduce = ht.table[i].size;
		ht.table[i] = linkList_clear(ht.table[i]);
		ht.size -= reduce;
	}
	return ht;
}

void* hashTable_getElement(HashTable ht, void* key)
{
	int index = hashTable_find(ht, key);
	if (index < 0) return NULL;
	return linkList_find(ht.table[index], key)->element;
}



