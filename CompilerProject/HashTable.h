/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include "linklist.h"

typedef struct
{
	int size;
	int hashTableSize;
	LinkList* table;
	int(*hashFunction) (void* value, int hashTableSize);
	int(*comparator) (void* value1, void* value2);
} HashTable;

HashTable hashTable_createNew(int hashTableSize, int(*comparator) (void*, void*), int(*hashFunction) (void* value, int hashTableSize));
int hashTable_find(HashTable ht, void* value);
//int symbolTable_find(HashTable ht, Symbol s);
HashTable hashTable_remove(HashTable ht, void* value);
HashTable hashTable_add(HashTable ht, void* value);
HashTable hashTable_clear(HashTable ht);
void* hashTable_getElement(HashTable ht, void* key);

#endif