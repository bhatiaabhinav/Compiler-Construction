/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#include "compiler.h"
#include <string.h>
#include "SymbolTableDef.h"

int symbolCompareByName(void* s1, void *s2)
{
	return strcmp(((Symbol*)s1)->symbolName, ((Symbol*)s2)->symbolName) == 0;
}

int hashFunctionForSymbols(void* sym, int hashTableSize)
{
	int sum = 0;
	int i = 0;
	while (((Symbol*)sym)->symbolName[i])
	{
		sum += (int)(((Symbol*)sym)->symbolName[i]);
		i++;
	}
	return sum % hashTableSize;
}



int symbolCompareByID(void* s1, void* s2)
{
	return ((Symbol*)s1)->symbolID == ((Symbol*)s2)->symbolID && ((Symbol*)s1)->isNonTerminal == ((Symbol*)s2)->isNonTerminal;
}

int compareByReference(void* v1, void* v2)
{
	return v1 == v2;
}

int typeComparator(void* t1, void* t2)
{
	return *((Type*)t1) == *((Type*)t2);
}

int booleanComparator(void* b1, void* b2)
{
	return *((Boolean*)b1) == *((Boolean*)b2);
}