/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#include "ASTDef.h"
#include "SymbolTable.h"
#include "compiler.h"
#include <string.h>

LinkList listOfSymbols; //LinkList<SymbolTableEntry*>
LinkList listOfFunctions; //LinkList<FunctionSymbolTableEntry*>
Boolean printSemanticErrors = TRUE;

int _typeExtractorandChecker(TreeLink subTree);

int varDeclaredInThisScope(TreeLink subTree)
{
	SymbolTableEntry entry;
	int index;
	ASTNodeData* data = (ASTNodeData*)subTree->data;
	strcpy(entry.tkInfo.lexeme, data->tokenInfo.lexeme);
	index = hashTable_find(*((HashTable*)data->symbolTables.head->element), &entry);
	return index >= 0;
}

SymbolTableEntry* findSymbolTableEntry(ASTNodeData* data)
{
	SymbolTableEntry dummyEntry;
	SymbolTableEntry* realEntry = NULL;
	LinkListNode* temp;
	
	dummyEntry.tkInfo = data->tokenInfo;
	for (temp = data->symbolTables.head; temp != NULL; temp = temp->next)
	{
		SymbolTable* table = (SymbolTable*)temp->element;
		realEntry = (SymbolTableEntry*)hashTable_getElement(*table, &dummyEntry);
		if (realEntry != NULL) return realEntry;
	}
	return realEntry;
}

FunctionSymbolTableEntry* findFunctionSymbolTableEntry(ASTNodeData* data)
{
	FunctionSymbolTableEntry dummyEntry;
	FunctionSymbolTableEntry* realEntry = NULL;
	LinkListNode* temp;

	dummyEntry.tkInfo = data->tokenInfo;
	for (temp = data->functionSymbolTables.head; temp != NULL; temp = temp->next)
	{
		FunctionSymbolTable* table = (FunctionSymbolTable*)temp->element;
		realEntry = (FunctionSymbolTableEntry*)hashTable_getElement(*table, &dummyEntry);
		if (realEntry != NULL) return realEntry;
	}
	return realEntry;
}

Boolean functionDeclared(TreeLink subtree)
{
	LinkListNode* temp; int index;
	FunctionSymbolTableEntry entry;
	ASTNodeData* data = (ASTNodeData*)subtree->data;
	ASTNodeData* childData = (ASTNodeData*)subtree->children[1]->data; //The FUNID child's data
	entry.tkInfo = childData->tokenInfo;
	
	for (temp = data->functionSymbolTables.head; temp != NULL; temp = temp->next)
	{
		FunctionSymbolTable* funcSymbolTable = (FunctionSymbolTable*)temp->element;
		index = hashTable_find(*funcSymbolTable, &entry);
		if (index > 0) return TRUE;
	}
	return FALSE;
}

int processDeclarationStmt(TreeLink subTree)
{
	ASTNodeData* data = (ASTNodeData*)subTree->data;
	char* strType = ((ASTNodeData*)subTree->children[0]->data)->symbol->symbolName;
	char* dataTypeStrings[4] = { "INT", "REAL", "STRING", "MATRIX" };
	Type dataTypes[4] = { TYPE_INT, TYPE_REAL, TYPE_STRING, TYPE_MATRIX };

	int i;
	SymbolTableEntry* entry;
	for (i = 0; i < 4; i++)
	{
		if (strcmp(strType, dataTypeStrings[i]) == 0)
		{
			int childID;
			Boolean errorExists = FALSE;
			data->type = dataTypes[i];
			for (childID = 1; childID < subTree->no_of_children; childID++)
			{
				if (findSymbolTableEntry((ASTNodeData*)subTree->children[childID]->data) != NULL)
				{
					if (printSemanticErrors) printf("\nLine %d: Redeclaration of variable %s", ((ASTNodeData*)subTree->children[childID]->data)->tokenInfo.lineNo,
						((ASTNodeData*)subTree->children[childID]->data)->tokenInfo.lexeme);
					errorExists = TRUE;
					continue;
				}
				((ASTNodeData*)subTree->children[childID]->data)->type = dataTypes[i];
				entry = createSymbolTableEntry();
				entry->tkInfo = ((ASTNodeData*)subTree->children[childID]->data)->tokenInfo;
				entry->type = ((ASTNodeData*)subTree->children[childID]->data)->type;
				entry->scope = data->scope;
				*((HashTable*)(data->symbolTables.head->element)) = hashTable_add(*((HashTable*)(data->symbolTables.head->element)), entry);
				listOfSymbols = linkList_addToTail(listOfSymbols, entry);
			}

			if (errorExists) return 0;
			break;
		}
	}

	return 1;
}

int handleIDCase(TreeLink subTree)
{
	SymbolTableEntry entry;
	ASTNodeData* data = (ASTNodeData*)subTree->data;
	HashTable* currentTable;
	LinkListNode* itr;
	SymbolTableEntry* found;

	strcpy(entry.tkInfo.lexeme, data->tokenInfo.lexeme);
	
	for (itr = data->symbolTables.head; itr != NULL; itr = itr->next)
	{
		currentTable = ((HashTable*)itr->element);
		found = (SymbolTableEntry*)hashTable_getElement(*currentTable, &entry);
		if (found == NULL) continue;
		data->type = found->type;
		data->numOfRows = found->numRows;
		data->numOfCols = found->numCols;
		data->stringSize = found->stringSize;
		return 1;
	}

	if (printSemanticErrors) printf("\nLine %d: Use of undeclared identifier %s", data->tokenInfo.lineNo, data->tokenInfo.lexeme);
	return 0;
}

void markAsAssigned(TreeLink subTree)
{
	SymbolTableEntry* entry = findSymbolTableEntry((ASTNodeData*)subTree->data);
	ASTNodeData* scopeData = (ASTNodeData*)entry->scope->data;
	FunctionSymbolTableEntry* nearestScopeFunEntry;
	if (strcmp(scopeData->symbol->symbolName, "<mainFunction>") == 0) return;
	nearestScopeFunEntry = findFunctionSymbolTableEntry((ASTNodeData*)entry->scope->children[1]->data);
	nearestScopeFunEntry->unassignedParameters = linkList_remove(nearestScopeFunEntry->unassignedParameters, entry);
}

Boolean handleFunctionDefCase(TreeLink subTree)
{
	ASTNodeData* data = (ASTNodeData*)subTree->data;
	ASTNodeData* childData = (ASTNodeData*)subTree->children[1]->data;
	FunctionSymbolTable* parentScopeFunctionSymbolTable = (FunctionSymbolTable*)data->functionSymbolTables.head->next->element;
	SymbolTable* thisScopeSymbolTable = (SymbolTable*)data->symbolTables.head->element;
	FunctionSymbolTableEntry* funEntry;
	int i;
	Boolean errorExists = FALSE;

	if (functionDeclared(subTree))
	{
		if (printSemanticErrors) printf("\nLine %d: Function %s redefinition. Function overloading is not supported.", ((ASTNodeData*)subTree->children[1]->data)->tokenInfo.lineNo, ((ASTNodeData*)subTree->children[1]->data)->tokenInfo.lexeme);
		return FALSE;
	}

	funEntry = createFunctionTableEntry();
	funEntry->tkInfo = childData->tokenInfo;
	*parentScopeFunctionSymbolTable = hashTable_add(*parentScopeFunctionSymbolTable, funEntry);
	listOfFunctions = linkList_addToTail(listOfFunctions, funEntry);
	
	//Handle return parameter list:
	for (i = 0; i < subTree->children[0]->no_of_children; i += 2)
	{
		ASTNodeData* typeNodeData = (ASTNodeData*)subTree->children[0]->children[i]->data;
		ASTNodeData* idNodeData = (ASTNodeData*)subTree->children[0]->children[i + 1]->data;
		char* dataTypeStrings[4] = { "INT", "REAL", "STRING", "MATRIX" };
		Type dataTypes[4] = { TYPE_INT, TYPE_REAL, TYPE_STRING, TYPE_MATRIX };
		int j;
		
		for (j = 0; j < 4; j++)
		{
			if (strcmp(typeNodeData->symbol->symbolName, dataTypeStrings[j])==0)
			{
				typeNodeData->type = idNodeData->type = dataTypes[j];
				funEntry->returnTypes = linkList_addToTailByValue(funEntry->returnTypes, &dataTypes[j], sizeof(Type));
				data->vectorType = linkList_addToTailByValue(data->vectorType, &dataTypes[j], sizeof(Type));

				if (findSymbolTableEntry((ASTNodeData*)subTree->children[0]->children[i + 1]->data) != NULL)
				{
					if (printSemanticErrors) printf("\nLine %d: Parameter %s redeclaration", idNodeData->tokenInfo.lineNo, idNodeData->tokenInfo.lexeme);
					errorExists = TRUE;
				}
				else
				{
					SymbolTableEntry* entry = createSymbolTableEntry();
					entry->tkInfo = idNodeData->tokenInfo;
					entry->type = dataTypes[j];
					entry->scope = data->scope;
					*thisScopeSymbolTable = hashTable_add(*thisScopeSymbolTable, entry);
					funEntry->unassignedParameters = linkList_add(funEntry->unassignedParameters, entry);
					listOfSymbols = linkList_addToTail(listOfSymbols, entry);
				}
				break;
			}
		}
	}

	//Handle input parameter list:
	for (i = 0; i < subTree->children[2]->no_of_children; i += 2)
	{
		ASTNodeData* typeNodeData = (ASTNodeData*)subTree->children[2]->children[i]->data;
		ASTNodeData* idNodeData = (ASTNodeData*)subTree->children[2]->children[i + 1]->data;
		char* dataTypeStrings[4] = { "INT", "REAL", "STRING", "MATRIX" };
		Type dataTypes[4] = { TYPE_INT, TYPE_REAL, TYPE_STRING, TYPE_MATRIX };
		int j;

		
		for (j = 0; j < 4; j++)
		{
			if (strcmp(typeNodeData->symbol->symbolName, dataTypeStrings[j]) == 0)
			{
				funEntry->inputTypes = linkList_addToTailByValue(funEntry->inputTypes, &dataTypes[j], sizeof(Type));
				typeNodeData->type = idNodeData->type = dataTypes[j];
				if (findSymbolTableEntry((ASTNodeData*)subTree->children[2]->children[i + 1]->data) != NULL)
				{
					if (printSemanticErrors) printf("\nLine %d: Parameter %s redeclaration", idNodeData->tokenInfo.lineNo, idNodeData->tokenInfo.lexeme);
					errorExists = TRUE;
				}
				else
				{
					SymbolTableEntry* entry = createSymbolTableEntry();	
					entry->tkInfo = idNodeData->tokenInfo;
					entry->type = dataTypes[j];
					entry->scope = data->scope;
					*thisScopeSymbolTable = hashTable_add(*thisScopeSymbolTable, entry);
					listOfSymbols = linkList_addToTail(listOfSymbols, entry);
				}
				break;
			}
		}
	}

	if (_typeExtractorandChecker(subTree->children[3]) == FALSE) return FALSE;
	
	return TRUE;
}

int handleArithmaticOp(TreeLink subTree)
{
	ASTNodeData* data = (ASTNodeData*)subTree->data;
	Type lhsType, rhsType;
	int errorExists = 0;
	int lhsCols, rhsCols;
	int lhsRows, rhsRows;

	int r1 = _typeExtractorandChecker(subTree->children[0]);
	int r2 = _typeExtractorandChecker(subTree->children[1]);
	if (r1 == 0 || r2 == 0) return 0;
	lhsType = ((ASTNodeData*)subTree->children[0]->data)->type;
	rhsType = ((ASTNodeData*)subTree->children[1]->data)->type;

	if (lhsType != rhsType)
	{
		errorExists = 1;
		if (printSemanticErrors) printf("\nLine %d: Type mismatch for operator %s", data->tokenInfo.lineNo, data->tokenInfo.lexeme);
	}
	else if (lhsType == TYPE_STRING && (strcmp(data->symbol->symbolName, "MINUS") == 0 ||
		strcmp(data->symbol->symbolName, "DIV") == 0 ||
		strcmp(data->symbol->symbolName, "MUL") == 0))
	{
		if (printSemanticErrors) printf("\Line %d: Operator %s cannot be applied to strings.", data->tokenInfo.lineNo, data->tokenInfo.lexeme);
		errorExists = 1;
	}
	else if (lhsType == TYPE_MATRIX && (strcmp(data->symbol->symbolName, "DIV") == 0 ||
		strcmp(data->symbol->symbolName, "MUL") == 0))
	{
		if (printSemanticErrors) printf("\nLine %d: Operator %s cannot be applied to matrices.", data->tokenInfo.lineNo, data->tokenInfo.lexeme);
		errorExists = 1;
	}
	else
	{
		if (lhsType == TYPE_MATRIX)
		{
			lhsCols = ((ASTNodeData*)subTree->children[0]->data)->numOfCols;
			lhsRows = ((ASTNodeData*)subTree->children[0]->data)->numOfRows;
			rhsCols = ((ASTNodeData*)subTree->children[1]->data)->numOfCols;
			rhsRows = ((ASTNodeData*)subTree->children[1]->data)->numOfRows;
			if (lhsCols != rhsCols || lhsRows != rhsRows)
			{
				if (printSemanticErrors) printf("\nLine %d: Operator %s cannot be applied to matrices of different dimensions", data->tokenInfo.lineNo, data->tokenInfo.lexeme);
				errorExists = 1;
			}
			else
			{
				data->numOfCols = lhsCols;
				data->numOfRows = lhsRows;
			}
		}
		else if (lhsType == TYPE_STRING)
		{
			int lhsStringSize = ((ASTNodeData*)subTree->children[0]->data)->stringSize;
			int rhsStringSize = ((ASTNodeData*)subTree->children[1]->data)->stringSize;
			data->stringSize = lhsStringSize + rhsStringSize;
			if (data->stringSize>20)
			{
				if (printSemanticErrors) printf("\nLine %d: String size overflow.", data->tokenInfo.lineNo);
				data->stringSize = 20;
				errorExists = 1;
			}
		}
	}

	if (errorExists)
	{
		data->type = TYPE_ERROR;
		return 0;
	}
	else data->type = lhsType;
	return 1;
}

int handleAssignOp(TreeLink subTree)
{
	ASTNodeData* data = (ASTNodeData*)subTree->data;
	Type lhsType, rhsType;
	LinkList lhsVectorType, rhsVectorType;
	int errorExists = 0;


	int r1 = _typeExtractorandChecker(subTree->children[0]);
	int r2 = _typeExtractorandChecker(subTree->children[1]);

	if (r1 == 0 || r2 == 0) return 0;

	lhsType = ((ASTNodeData*)subTree->children[0]->data)->type;
	lhsVectorType = ((ASTNodeData*)subTree->children[0]->data)->vectorType;
	rhsType = ((ASTNodeData*)subTree->children[1]->data)->type;
	rhsVectorType = ((ASTNodeData*)subTree->children[1]->data)->vectorType;


	if (lhsVectorType.size != rhsVectorType.size && !(lhsVectorType.size == 0 && rhsVectorType.size == 1))
	{
		if (printSemanticErrors) ("\nLine %d: Invalid assignment statement", data->tokenInfo.lineNo);
		errorExists = 1;
	}
	else
	{
		if (lhsVectorType.size == rhsVectorType.size)
		{
			if (lhsVectorType.size == 0)
			{
				if (lhsType != rhsType)
				{
					errorExists = 1;
					if (printSemanticErrors) printf("\nLine %d: Type mismatch", data->tokenInfo.lineNo);
				}
				else
				{
					if (lhsType == TYPE_MATRIX)
					{
						int rhsCols, rhsRows;
						SymbolTableEntry* lhsEntry;
						rhsCols = ((ASTNodeData*)subTree->children[1]->data)->numOfCols;
						rhsRows = ((ASTNodeData*)subTree->children[1]->data)->numOfRows;
						((ASTNodeData*)subTree->children[0]->data)->numOfCols = rhsCols;
						((ASTNodeData*)subTree->children[0]->data)->numOfRows = rhsRows;

						lhsEntry = findSymbolTableEntry((ASTNodeData*)subTree->children[0]->data);
						if (lhsEntry->numCols==0 || lhsEntry->numCols==rhsCols) lhsEntry->numCols = rhsCols;
						else
						{
							errorExists = 1;
							if (printSemanticErrors) printf("\nLine %d: Changing dimensions of matrix %s is not allowed", data->tokenInfo.lineNo, lhsEntry->tkInfo.lexeme);
						}
						if (lhsEntry->numRows == 0 || lhsEntry->numRows == rhsRows) lhsEntry->numRows = rhsRows;
						else
						{
							if (!errorExists && printSemanticErrors) printf("\nLine %d: Changing dimensions of matrix %s is not allowed", data->tokenInfo.lineNo, lhsEntry->tkInfo.lexeme);
							errorExists = 1;
						}
					}
					else if (lhsType == TYPE_STRING)
					{
						int rhsStringSize;
						SymbolTableEntry* lhsEntry;
						rhsStringSize = ((ASTNodeData*)subTree->children[1]->data)->stringSize;
						((ASTNodeData*)subTree->children[0]->data)->stringSize = rhsStringSize;

						lhsEntry = findSymbolTableEntry((ASTNodeData*)subTree->children[0]->data);
						lhsEntry->stringSize = rhsStringSize;
					}
					markAsAssigned(subTree->children[0]);
				}
			}
			else
			{
				LinkListNode *lhsItr = lhsVectorType.head, *rhsItr = rhsVectorType.head;
				int i;
				for (i = 0; i < lhsVectorType.size; i++)
				{
					if (typeComparator((Type*)lhsItr->element, (Type*)rhsItr->element) == 0)
					{
						errorExists = 1;
						if (printSemanticErrors) printf("\nLine %d: Type mismatch for output parameter #%d in assignment statement", data->tokenInfo.lineNo, i + 1);
					}
					markAsAssigned(subTree->children[0]->children[i]);
					lhsItr = lhsItr->next;
					rhsItr = rhsItr->next;
				}
			}
		}
		else
		{
			//One ID to the left and function call to the right
			if (lhsType != *((Type*)rhsVectorType.head->element))
			{
				errorExists = 1;
				if (printSemanticErrors) printf("\nLine %d: Type mismatch in functional call assignment statement", data->tokenInfo.lineNo);
			}
			markAsAssigned(subTree->children[0]);
		}
	}

	if (errorExists) return 0;

	return 1;
}

int handleFunctionCall(TreeLink subTree)
{
	Boolean errorExists = TRUE;
	LinkListNode* temp;
	FunctionSymbolTableEntry dummyEntry;
	FunctionSymbolTableEntry* funcEntry;
	ASTNodeData* data = (ASTNodeData*)subTree->data;
	ASTNodeData* childData = (ASTNodeData*)subTree->children[0]->data; //The FUNID child's data
	ASTNodeData* scopeData = (ASTNodeData*)data->scope->data;
	Type* expectedParamType;
	LinkListNode* itr;
	int i;

	dummyEntry.tkInfo = childData->tokenInfo;

	for (temp = data->functionSymbolTables.head; temp != NULL; temp = temp->next)
	{
		FunctionSymbolTable* funcSymbolTable = (FunctionSymbolTable*)temp->element;
		funcEntry = (FunctionSymbolTableEntry*)hashTable_getElement(*funcSymbolTable, &dummyEntry);
		if (funcEntry == NULL) continue;

		if (strcmp("<mainFunction>", scopeData->symbol->symbolName) != 0 &&
			strcmp(funcEntry->tkInfo.lexeme, ((ASTNodeData*)data->scope->children[1]->data)->tokenInfo.lexeme) == 0)
		{
			errorExists = TRUE;
			if (printSemanticErrors) printf("\nline %d: Recursion is not allowed.", ((ASTNodeData*)subTree->children[1]->data)->tokenInfo.lineNo);
			return 0;
		}

		//Now we have a legit function entry

		errorExists = FALSE;
		data->vectorType = linkList_copy(funcEntry->returnTypes);

		//Now we need to check whether the passed parameters are of correct type

		itr = funcEntry->inputTypes.head;
		if (subTree->no_of_children - 1 != funcEntry->inputTypes.size)
		{
			errorExists = TRUE;
			if (printSemanticErrors) printf("\nLine %d: number of input parameters in function call to function %s does not match with the number of formal input parameters",
				((ASTNodeData*)subTree->children[0]->data)->tokenInfo.lineNo, funcEntry->tkInfo.lexeme);
		}
		else
		{
			for (i = 1; i < subTree->no_of_children; i++)
			{
				Type paramType;
				if (_typeExtractorandChecker(subTree->children[i]) == 0) return 0;
				paramType = ((ASTNodeData*)subTree->children[i]->data)->type;
				expectedParamType = (Type*)itr->element;
				if (paramType != *expectedParamType)
				{
					if (printSemanticErrors)printf("\nLine %d: Parameter %s is of incorrect type when invoking function %s", ((ASTNodeData*)subTree->children[i]->data)->tokenInfo.lineNo,
						((ASTNodeData*)subTree->children[i]->data)->tokenInfo.lexeme,
						funcEntry->tkInfo.lexeme);
					errorExists = TRUE;
				}
				itr = itr->next;
			}
		}

		if (errorExists) return 0;
		else return 1;
	}

	if (printSemanticErrors) printf("\nLine %d: Use of undefined function %s", childData->tokenInfo.lineNo, childData->tokenInfo.lexeme);
	return 0;
}

int _typeExtractorandChecker(TreeLink subTree)
{
	ASTNodeData* data = (ASTNodeData*)subTree->data;
	if (strcmp(data->symbol->symbolName, "<declarationStmt>") == 0)
	{
		return processDeclarationStmt(subTree);
	}
	else if (strcmp(data->symbol->symbolName, "NUM") == 0)
	{
		data->type = TYPE_INT;
	}
	else if (strcmp(data->symbol->symbolName, "RNUM") == 0)
	{
		data->type = TYPE_REAL;
	}
	else if (strcmp(data->symbol->symbolName, "STR") == 0)
	{
		data->type = TYPE_STRING;
		data->stringSize = strlen(data->tokenInfo.lexeme) - 2;
		if (data->stringSize>20)
		{
			if (printSemanticErrors) printf("\nLine %d: Specified string is too long", data->tokenInfo.lineNo);
			data->stringSize = 20;
			return 0;
		}
	}
	else if (strcmp(data->symbol->symbolName, "<matrix>") == 0)
	{
		int i, cols;
		data->type = TYPE_MATRIX;
		data->numOfRows = subTree->no_of_children;
		data->numOfCols = cols = subTree->children[0]->no_of_children;
		for (i = 1; i < subTree->no_of_children; i++)
		{
			if (subTree->children[i]->no_of_children != cols)
			{
				if (printSemanticErrors) printf("\nLine %d: Bad matrix specification", ((ASTNodeData*)subTree->children[0]->children[0]->data)->tokenInfo.lineNo);
				data->numOfCols = cols;
				return 0;
			}
		}
		if (data->numOfRows > 10 || data->numOfCols>10)
		{
			if(printSemanticErrors) printf("\nLine %d: Too many rows or columns.", ((ASTNodeData*)subTree->children[0]->children[0]->data)->tokenInfo.lineNo);
			if (data->numOfRows > 10) data->numOfRows = 10;
			if (data->numOfCols > 10) data->numOfCols = 10;
			return 0;
		}
		return 1;
	}
	else if (strcmp(data->symbol->symbolName, "<ID_or_matrixElement>") == 0)
	{
		//this is a matrix element.
		data->type = TYPE_INT;
	}
	else if (strcmp(data->symbol->symbolName, "PLUS") == 0 || strcmp(data->symbol->symbolName, "MINUS") == 0 ||
		strcmp(data->symbol->symbolName, "DIV") == 0 || strcmp(data->symbol->symbolName, "MUL") == 0)
	{
		return handleArithmaticOp(subTree);
	}
	else if (strcmp(data->symbol->symbolName, "ID") == 0)
	{
		return handleIDCase(subTree);
	}
	else if (strcmp(data->symbol->symbolName, "ASSIGNOP") == 0)
	{
		return handleAssignOp(subTree);
	}
	else if (strcmp(data->symbol->symbolName, "<leftHandSide_listVar>") == 0)
	{
		int i;
		Type childType;
		for (i = 0; i < subTree->no_of_children; i++)
		{
			if (_typeExtractorandChecker(subTree->children[i]) == 0) return 0;
			childType = ((ASTNodeData*)subTree->children[i]->data)->type;
			data->vectorType = linkList_addToTailByValue(data->vectorType, &childType, sizeof(Type));
		}
		return 1;
	}

	else if (strcmp(data->symbol->symbolName, "AND") == 0 || strcmp(data->symbol->symbolName, "OR") == 0 || strcmp(data->symbol->symbolName, "NOT") == 0)
	{
		if (_typeExtractorandChecker(subTree->children[0]) == 0) return 0;
		if (strcmp(data->symbol->symbolName, "NOT") != 0)
		{
			if (_typeExtractorandChecker(subTree->children[1]) == 0) return 0;
		}

		data->type = TYPE_BOOLEAN;
		return 1;
	}
	else if (strcmp(data->symbol->symbolName, "LE") == 0 || strcmp(data->symbol->symbolName, "LT") == 0 || strcmp(data->symbol->symbolName, "GT") == 0 ||
		strcmp(data->symbol->symbolName, "GE") == 0 || strcmp(data->symbol->symbolName, "EQ") == 0 || strcmp(data->symbol->symbolName, "NE") == 0)
	{
		Type lhsType, rhsType;
		int errorExists = 0;
		if (_typeExtractorandChecker(subTree->children[0]) == 0) return 0;
		if (_typeExtractorandChecker(subTree->children[1]) == 0) return 0;
		lhsType = ((ASTNodeData*)subTree->children[0]->data)->type;
		rhsType = ((ASTNodeData*)subTree->children[1]->data)->type;

		
		if (lhsType != rhsType) errorExists = 1;
		if (!(lhsType == TYPE_INT || lhsType == TYPE_REAL)) errorExists = 1;

		if (errorExists)
		{
			if (printSemanticErrors) printf("\nLine %d: Type mismatch for operator %s", data->tokenInfo.lineNo, data->tokenInfo.lexeme);
			return 0;
		}

		data->type = TYPE_BOOLEAN;
		return 1;
	}
	else if (strcmp(data->symbol->symbolName, "SIZE") == 0)
	{
		Type childType;
		if (_typeExtractorandChecker(subTree->children[0]) == 0) return 0;
		childType = ((ASTNodeData*)subTree->children[0]->data)->type;

		if (childType == TYPE_STRING)
		{
			data->type = TYPE_INT;
		}
		else if (childType == TYPE_MATRIX)
		{
			data->type = TYPE_VECTOR2I;
			Type t = TYPE_INT;
			data->vectorType = linkList_addToTailByValue(data->vectorType, &t, sizeof(t));
			data->vectorType = linkList_addToTailByValue(data->vectorType, &t, sizeof(t));
		}
		else
		{
			if (printSemanticErrors) printf("\nLine %d: Inavlid type for sizeof operator @.", data->tokenInfo.lineNo);
			return 0;
		}
		return 1;
	}
	else if (strcmp(data->symbol->symbolName, "<functionDef>") == 0)
	{
		return (int)handleFunctionDefCase(subTree);
	}
	else if(strcmp(data->symbol->symbolName, "<funCallStmt>") == 0)
	{
		return handleFunctionCall(subTree);
	}
			
	else
	{
		int i;
		int errorExists = 0;
		for (i = 0; i < subTree->no_of_children; i++)
		{
			if (_typeExtractorandChecker(subTree->children[i]) == 0) errorExists = 1;
		}
		if (errorExists) return 0;
	}

	return 1;
}

void calculateOffsets()
{
	LinkListNode *varItr;
	int* offsetTobeIncremented;
	int mainOffset = 0;

	for (varItr = listOfSymbols.head; varItr != NULL; varItr = varItr->next)
	{
		SymbolTableEntry* entry = (SymbolTableEntry*)varItr->element;
		ASTNodeData* scopeData = (ASTNodeData*)entry->scope->data;
		FunctionSymbolTableEntry* nearestScopeFunEntry;

		if (strcmp(scopeData->symbol->symbolName, "<mainFunction>") == 0) offsetTobeIncremented = &mainOffset;
		else
		{
			nearestScopeFunEntry = findFunctionSymbolTableEntry((ASTNodeData*)entry->scope->children[1]->data);
			offsetTobeIncremented = &(nearestScopeFunEntry->offset);
		}

		entry->offset = *offsetTobeIncremented;

		switch (entry->type)
		{
		case TYPE_INT:(*offsetTobeIncremented) += 2; break;
		case TYPE_REAL:(*offsetTobeIncremented) += 4; break;
		case TYPE_STRING:
			(*offsetTobeIncremented) += entry->stringSize;
			if (entry->stringSize == 0) printf("\nLine %d: Warning: Memory layout of uninitialised string %s cannot be calculated at compile time.", entry->tkInfo.lineNo, entry->tkInfo.lexeme);
			break;
		case TYPE_MATRIX:
			(*offsetTobeIncremented) += 2 * entry->numRows*entry->numCols;
			if (entry->numCols == 0 && entry->numRows == 0) printf("\nLine %d: Warning: Memory layout of uninitialised matrix %s cannot be calculated at compile time.", entry->tkInfo.lineNo, entry->tkInfo.lexeme);
			break;
		default:
			break;
		}

	}

}

int typeExtractorandChecker(Tree ast)
{
	int result;
	LinkListNode* itr;
	listOfSymbols = linkList_createNew(compareByReference);
	listOfFunctions = linkList_createNew(compareByReference);
	result = _typeExtractorandChecker(ast.root);
	
	for (itr = listOfFunctions.head; itr != NULL; itr = itr->next)
	{
		FunctionSymbolTableEntry* funEntry = (FunctionSymbolTableEntry*)itr->element;
		if (funEntry->unassignedParameters.size > 0)
		{
			result = 0;
			if (printSemanticErrors) printf("\nLine %d: All return parameters of function %s are not assigned.", funEntry->tkInfo.lineNo, funEntry->tkInfo.lexeme);
		}
	}

	calculateOffsets();

	return result;
}

void printSymbolTable()
{
	LinkListNode* itr;
	char* typeNames[4] = { "Integer", "Real", "String", "Matrix" };

	printf("\n\n%-20s \t %-20s \t %10s \t %10s\n", "Identifier", "scope", "type", "offset");
	printf("------------------------------------------------------------------------------\n");

	for (itr = listOfSymbols.head; itr != NULL; itr = itr->next)
	{
		SymbolTableEntry* entry = (SymbolTableEntry*)itr->element;
		if (entry->type != TYPE_MATRIX)
		{
			if (strcmp(((ASTNodeData*)entry->scope->data)->symbol->symbolName, "<mainFunction>") != 0)
			{
				printf("\n%-20s \t %-20s \t %10s \t %4d", entry->tkInfo.lexeme, ((ASTNodeData*)entry->scope->children[1]->data)->tokenInfo.lexeme,
					typeNames[(int)entry->type], entry->offset);
			}
			else
			{
				printf("\n%-20s \t %-20s \t %10s \t %4d", entry->tkInfo.lexeme, "_main",
					typeNames[(int)entry->type], entry->offset);
			}
		}
		else
		{
			if (strcmp(((ASTNodeData*)entry->scope->data)->symbol->symbolName, "<mainFunction>") != 0)
			{
				printf("\n%-20s \t %-20s \t %10s(%dx%d) \t %4d", entry->tkInfo.lexeme, ((ASTNodeData*)entry->scope->children[1]->data)->tokenInfo.lexeme,
					typeNames[(int)entry->type], entry->numRows,entry->numCols, entry->offset);
			}
			else
			{
				printf("\n%-20s \t %-20s \t %10s(%dx%d) \t %4d", entry->tkInfo.lexeme, "_main",
					typeNames[(int)entry->type], entry->numRows, entry->numCols, entry->offset);
			}
		}
	}
}

void turnOnReportingSemanticErrors(Boolean onOrOff)
{
	printSemanticErrors = onOrOff;
}