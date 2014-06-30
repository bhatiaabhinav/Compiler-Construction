/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#include "SymbolTable.h"
#include "compiler.h"
#include "AST.h"

SymbolTableEntry* createSymbolTableEntry()
{
	SymbolTableEntry* entry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
	entry->offset = 0;
	entry->scope = 0;
	entry->type = TYPE_INT;
	entry->numRows = 0;
	entry->numCols = 0;
	entry->stringSize = 0;
	return entry;
}

FunctionSymbolTableEntry* createFunctionTableEntry()
{
	FunctionSymbolTableEntry* entry = (FunctionSymbolTableEntry*)malloc(sizeof(FunctionSymbolTableEntry));
	entry->returnTypes = linkList_createNew(typeComparator);
	entry->inputTypes = linkList_createNew(typeComparator);
	entry->unassignedParameters = linkList_createNew(compareByReference);
	entry->offset = 0;
	return entry;
}

void createAndCopy(TreeLink root, TreeLink child)
{
	Stack s;
	//s = ((ASTNodeData*)child->data)->symbolTabl
	SymbolTable* newHashTable;
	newHashTable = (SymbolTable*)malloc(sizeof(HashTable));
	(*newHashTable) = hashTable_createNew(SYMBOL_TABLE_SIZE, symbolTableEntryComparator, hashFunctionForSymbolTableEntries);
	s = linkList_copy(((ASTNodeData*)root->data)->symbolTables);
	//s = stack_push(s, &newHashTable);
	
	((ASTNodeData*)child->data)->symbolTables = s;
	((ASTNodeData*)child->data)->symbolTables = stack_push(((ASTNodeData*)child->data)->symbolTables, newHashTable);
	

	//s = ((ASTNodeData*)child->data)->symbolTabl
	FunctionSymbolTable* newFuncHashTable;
	newFuncHashTable = (FunctionSymbolTable*)malloc(sizeof(HashTable));
	(*newFuncHashTable) = hashTable_createNew(FUNCTION_SYMBOL_TABLE_SIZE, functionSymbolTableEntryComparator, hashFunctionForFunctionSymbolTableEntries);
	s = linkList_copy(((ASTNodeData*)root->data)->functionSymbolTables);
	//s = stack_push(s, &newHashTable);

	((ASTNodeData*)child->data)->functionSymbolTables = s;
	((ASTNodeData*)child->data)->functionSymbolTables = stack_push(((ASTNodeData*)child->data)->functionSymbolTables, newFuncHashTable);

}


int needScope(TreeLink node)
{
	if (strcmp(((ASTNodeData*)node->data)->symbol->symbolName, "<functionDef>") == 0)
		return 1;
	else return 0;
}

void  _createSymbolTables(TreeLink root)
{	
	int i;
	for (i = 0; i < root->no_of_children; i++)
	{
		if (needScope(root->children[i]))
		{
			createAndCopy(root, root->children[i]);
			((ASTNodeData*)root->children[i]->data)->scope = root->children[i];
		}
		else
		{
			((ASTNodeData*)root->children[i]->data)->symbolTables = ((ASTNodeData*)root->data)->symbolTables;
			((ASTNodeData*)root->children[i]->data)->functionSymbolTables = ((ASTNodeData*)root->data)->functionSymbolTables;
			((ASTNodeData*)root->children[i]->data)->scope = ((ASTNodeData*)root->data)->scope;
		}

		_createSymbolTables(root->children[i]);
	}
}

void createSymbolTables(Tree ast)
{
	FunctionSymbolTable* mainFunctionTable = (FunctionSymbolTable*)malloc(sizeof(FunctionSymbolTable));
	*mainFunctionTable=	hashTable_createNew(FUNCTION_SYMBOL_TABLE_SIZE, functionSymbolTableEntryComparator, hashFunctionForFunctionSymbolTableEntries);
	

	SymbolTable* mainTable = (SymbolTable*)malloc(sizeof(SymbolTable));
	*mainTable = hashTable_createNew(SYMBOL_TABLE_SIZE, symbolTableEntryComparator, hashFunctionForSymbolTableEntries);
	
	((ASTNodeData*)ast.root->data)->symbolTables = stack_createNew(compareByReference);
	((ASTNodeData*)ast.root->data)->functionSymbolTables = stack_createNew(compareByReference);
	// pointer part to be seen
	//Stack mainStack = ((ASTNodeData*)ast.root->data)->symbolTables;
	((ASTNodeData*)ast.root->data)->symbolTables = stack_push(((ASTNodeData*)ast.root->data)->symbolTables, mainTable);
	((ASTNodeData*)ast.root->data)->functionSymbolTables = stack_push(((ASTNodeData*)ast.root->data)->functionSymbolTables, mainFunctionTable);

	((ASTNodeData*)ast.root->data)->scope = ast.root;

	_createSymbolTables(ast.root);

}

int symbolTableEntryComparator(void* entry1, void* entry2)
{
	return strcmp(((SymbolTableEntry*)entry1)->tkInfo.lexeme, ((SymbolTableEntry*)entry2)->tkInfo.lexeme) == 0;
}

int hashFunctionForSymbolTableEntries(void* entry, int hashTableSize)
{
	char* str = ((SymbolTableEntry*)entry)->tkInfo.lexeme;
	int i = 0;
	int sum = 0;
	while (str[i] != '\0')
	{
		sum += (int)str[i];
		i++;
	}

	return sum%hashTableSize;
}

int functionSymbolTableEntryComparator(void* entry1, void* entry2)
{
	return strcmp(((FunctionSymbolTableEntry*)entry1)->tkInfo.lexeme, ((FunctionSymbolTableEntry*)entry2)->tkInfo.lexeme) == 0;
}
int hashFunctionForFunctionSymbolTableEntries(void* entry, int hashTableSize)
{
	char* str = ((FunctionSymbolTableEntry*)entry)->tkInfo.lexeme;
	int i = 0;
	int sum = 0;
	while (str[i] != '\0')
	{
		sum += (int)str[i];
		i++;
	}

	return sum%hashTableSize;
}