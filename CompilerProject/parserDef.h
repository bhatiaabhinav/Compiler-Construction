/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _PARSERDEF_H_
#define _PARSERDEF_H_

#include "definitions.h"

typedef Tree ParseTree;

typedef struct
{
	Grammar grammar;
	Rule*** parseTable; //An entry in a parse table cell stores pointer to a rule.
	Tree parseTree;
} Parser;

typedef Rule*** ParseTable;

typedef struct
{
	UsefulNess usefulNess;
	Rule* expandsToRule; //The rule for which this symbol is LHS
	Rule* expandsFrom;
	Symbol* symbol;
	TokenInfo tokenInfo;
} ParseTreeNodeData; //Data of a node in a parse tree points to a structure of this type.

#endif