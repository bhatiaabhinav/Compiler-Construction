/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _TYPECHECKER_H_
#define _TYPECHECKER_H_

#include "definitions.h"
#include "SymbolTable.h"

int typeExtractorandChecker(Tree ast);
SymbolTableEntry* findSymbolTableEntry(ASTNodeData* data);
void printSymbolTable();
void turnOnReportingSemanticErrors(Boolean onOrOff);

#endif