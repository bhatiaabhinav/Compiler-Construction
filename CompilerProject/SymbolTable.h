/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include "SymbolTableDef.h"

void createSymbolTables(Tree ast);
void createAndCopy(TreeLink root, TreeLink child);
void justCopy(TreeLink root, TreeLink child);
void  _createSymbolTables(TreeLink root);

SymbolTableEntry* createSymbolTableEntry();
FunctionSymbolTableEntry* createFunctionTableEntry();

int symbolTableEntryComparator(void* entry1, void* entry2);
int hashFunctionForSymbolTableEntries(void* entry, int hashTableSize);
int functionSymbolTableEntryComparator(void* entry1, void* entry2);
int hashFunctionForFunctionSymbolTableEntries(void* entry, int hashTableSize);

#endif