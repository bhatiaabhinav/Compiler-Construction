/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _SYMBOL_TABLE_DEF_
#define _SYMBOL_TABLE_DEF_

#include "definitions.h"

#define MAX_PARAMETERS_PER_FUNCTION 10
#include "AST.h"

typedef struct
{
	TokenInfo tkInfo;
	LinkList returnTypes; //LinkList<Type>
	LinkList inputTypes; //Linklist<Type>
	LinkList unassignedParameters; //LinkList<SymbolTableEntry*>
	int offset;
} FunctionSymbolTableEntry;

typedef HashTable FunctionSymbolTable;

typedef struct
{
	TokenInfo tkInfo;
	Type type;
	int offset;
	Scope scope;
	int numRows; //for matrix
	int numCols; //for matrix
	int stringSize; //for strings
}SymbolTableEntry;

typedef HashTable SymbolTable;

#endif
