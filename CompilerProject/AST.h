/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _AST_H_
#define _AST_H_

#include "ASTDef.h"
#include "parserDef.h"

Tree createAst(Parser parser);
Tree ASTConstructionDfs(TreeLink node);
void printAST(Tree ast);
int toPush(char* string);
void removeChild(TreeLink root, int i);
Tree clearAstDfs(TreeLink root, Tree ast);
Tree clearAst(Tree ast);

#endif