/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _ASTDEF_H_
#define _ASTDEF_H_

#include "definitions.h"

typedef Tree AST;
struct _ASTNodeData;
typedef struct _ASTNodeData ASTNodeData;

typedef TreeLink Scope;

struct _ASTNodeData
{
	Symbol* symbol;
	Stack symbolTables; //Stack<SymbolTable*>
	Stack functionSymbolTables; //Stack<FunctionSymbolTable*>
	TokenInfo tokenInfo;
	Type type;
	LinkList vectorType; //Linklist<Type>
	int numOfRows, numOfCols; //for matrix
	int stringSize; //for strings
	Scope scope;
	int offset; //only for <mainFunction> and <functionDef>
};

#endif