/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _COMPILER_H_
#define _COMPILER_H_
#include "CompilerDef.h"

int hashFunctionForSymbols(void* sym, int hashTableSize);
int symbolCompareByName(void* s1, void *s2);
int symbolCompareByID(void* s1, void* s2);
int compareByReference(void* v1, void* v2);
int typeComparator(void* t1, void* t2);
int booleanComparator(void* b1, void* b2);

#endif