/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#include "commons.h"
#include "constants.h"

typedef struct
{
	char lexeme[MAX_LEXEME_SIZE]; //The lexeme. e.g. hello
	char token[20]; //The token e.g. ID
	int lineNo; //The line no. at which this lexeme appeared.
} TokenInfo;

//This structure should be used mostly by reference
typedef struct
{
	char symbolName[MAX_WORD];
	int symbolID;
	int isNonTerminal;
} Symbol;

//This structure should be used mostly by reference
typedef struct
{
	int ruleNo; //1 based
	Symbol* lhs;
	LinkList rhsSymbols;
	int no_of_rhs;
} Rule;

typedef struct
{
	LinkList rules;
	HashSet terminalsSet;
	HashSet nonTerminalSet;
	Symbol* epsilon;
	Symbol* dollar;
} Grammar;


typedef enum
{
	useless, //No Edge
	forwarded, //Dashed Edge
	concrete //Edge
} UsefulNess;

typedef enum
{
	TYPE_INT,
	TYPE_REAL,
	TYPE_STRING,
	TYPE_MATRIX,
	TYPE_BOOLEAN,
	TYPE_VECTOR2I,
	TYPE_ERROR
} Type;

#endif