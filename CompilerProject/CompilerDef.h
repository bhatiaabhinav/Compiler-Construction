/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _LANGAUGEDEF_H_
#define _LANGUAGEDEF_H_

#include "parser.h"
#include "lexer.h"

struct _Compiler;

typedef struct _Compiler
{
	Parser parser;
	Lexer lexer;
} Compiler;



#endif