/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#include "definitions.h"
#include "parser.h"
#include "compiler.h"
#include "first.h"
#include "follow.h"

char usefullArray[NO_RULES][MAX_NO_RHS + 1];

Grammar populateRules(FILE *fp)
{
	Grammar grammar;
	LinkList allGrammarRules = linkList_createNew((int(*)(void*, void*))compareRulesByReference);
	char line[LINE_LENGTH];
	Rule* newRule;
	int SymbolCount = 0;
	int ruleNoInGrammar = 0;
	Symbol* dollar;

	HashSet nonTerminalSet = set_create(MAX_NO_SYMBOLS, (int(*)(void*, void*)) symbolCompareByName, hashFunctionForSymbols);
	HashSet terminalSet = set_create(MAX_NO_SYMBOLS, (int(*)(void*, void*))symbolCompareByName, hashFunctionForSymbols);

	Symbol* epsilon = (Symbol*)malloc(sizeof(Symbol));
	epsilon->symbolID = 0;
	epsilon->isNonTerminal = 0;
	strcpy(epsilon->symbolName, "e");
	grammar.epsilon = epsilon;
	terminalSet = set_add(terminalSet, epsilon);

	dollar = (Symbol*)malloc(sizeof(Symbol));
	dollar->symbolID = 1;
	dollar->isNonTerminal = 0;
	strcpy(dollar->symbolName, "$");
	grammar.dollar = dollar;
	terminalSet = set_add(terminalSet, dollar);

	while (1)
	{
		if (fscanf(fp, "%s", line) > 0)
		{
			Symbol* newSymbol;
			ruleNoInGrammar++;

			newRule = (Rule*)malloc(sizeof(Rule));
			newRule->lhs = NULL;
			newRule->no_of_rhs = 0;
			newRule->rhsSymbols = linkList_createNew((int(*)(void*, void*))symbolCompareByName);
			newRule->ruleNo = ruleNoInGrammar;

			newSymbol = (Symbol*)malloc(sizeof(Symbol));
			newSymbol->isNonTerminal = 1;
			strcpy(newSymbol->symbolName, line);
			

			// should compare with symbol name
			if (set_contains(nonTerminalSet, newSymbol))
			{
				newRule->lhs = (Symbol*)set_getElement(nonTerminalSet, newSymbol);
				free(newSymbol);
			}
			else
			{
				newSymbol->symbolID = nonTerminalSet.hashTable.size;
				nonTerminalSet = set_add(nonTerminalSet, newSymbol);
				newRule->lhs = newSymbol;
			}


			//now for RHS
			while (1)
			{
				fscanf(fp, "%s", line);

				if (strcmp(line, "->") == 0)
					continue;
				else
				{
					if (line[0] == '<')
					{
						newSymbol = (Symbol*)malloc(sizeof(Symbol));
						newSymbol->isNonTerminal = 1;
						strcpy(newSymbol->symbolName, line);


						/// should compare with symbol name
						if (set_contains(nonTerminalSet, newSymbol))
						{
							newRule->rhsSymbols = linkList_addToTail(newRule->rhsSymbols, set_getElement(nonTerminalSet, newSymbol));
							newRule->no_of_rhs = newRule->rhsSymbols.size;
							free(newSymbol);
						}

						else
						{
							newSymbol->symbolID = nonTerminalSet.hashTable.size;
							nonTerminalSet = set_add(nonTerminalSet, newSymbol);
							newRule->rhsSymbols = linkList_addToTail(newRule->rhsSymbols, newSymbol);

						}

					}

					else if (strcmp(line, ".") == 0)
					{
						allGrammarRules = linkList_addToTail(allGrammarRules, newRule);
						break;
					}

					else
					{ //this time its  a terminal

						newSymbol = (Symbol*)malloc(sizeof(Symbol));
						newSymbol->isNonTerminal = 0;
						strcpy(newSymbol->symbolName, line);


						/// should compare with symbol name
						if (set_contains(terminalSet, newSymbol))
						{
							newRule->rhsSymbols = linkList_addToTail(newRule->rhsSymbols, set_getElement(terminalSet, newSymbol));
							newRule->no_of_rhs = newRule->rhsSymbols.size;
							free(newSymbol);
						}

						else
						{
							newSymbol->symbolID = terminalSet.hashTable.size;
							terminalSet = set_add(terminalSet, newSymbol);
							newRule->rhsSymbols = linkList_addToTail(newRule->rhsSymbols, newSymbol);

						}
					}

				}

			}

		}

		else
			break;
	}

	grammar.nonTerminalSet = nonTerminalSet;
	grammar.terminalsSet = terminalSet;
	grammar.rules = allGrammarRules;
	return grammar;
}

void printRule(Rule* r)
{
	LinkListNode* rhsItr;
	printf("%s -> ", r->lhs->symbolName, r->lhs->symbolID);

	for (rhsItr = r->rhsSymbols.head; rhsItr != NULL; rhsItr = rhsItr->next)
	{
		Symbol* s = (Symbol*)rhsItr->element;
		printf("%s ", s->symbolName, s->symbolID);
	}
}

void printRules(Grammar g)
{
	LinkListNode* itr;
	for (itr = g.rules.head; itr != NULL; itr = itr->next)
	{
		Rule* currentRule = (Rule*)itr->element;
		printRule(currentRule);

		printf("\n");
	}
}

Parser parser_initialise(FILE* grammarInputFile, FILE* astRulesFile)
{
	Parser p;
	int row;

	printf("\nInitializing parser.....");

	printf("\nReading Grammar Rules...");
	p.grammar = populateRules(grammarInputFile);
	initUsefulArray(astRulesFile);
	printf("\nSuccessfully Read Grammar Rules :)");
	//printRules(p.grammar);

	//Allocate and clean memory for the parse table:
	p.parseTable = (Rule***)malloc(p.grammar.nonTerminalSet.hashTable.size*sizeof(Rule**));
	
	for (row = 0; row < p.grammar.nonTerminalSet.hashTable.size; row++)
	{
		int col;
		p.parseTable[row] = (Rule**)malloc(p.grammar.terminalsSet.hashTable.size*sizeof(Rule*));
		for (col = 0; col < p.grammar.terminalsSet.hashTable.size; col++)
		{
			p.parseTable[row][col] = NULL;
		}
	}

	printf("\nCreating parse table...");
	createParseTable(p.grammar, p.parseTable);
	printf("\nParse Table created :)\n");

	p.parseTree = tree_createNew();

	return p;
}

void createParseTable(Grammar grammar, ParseTable parseTable)
{
	LinkListNode* itr;
	HashSetIterator columnIterator;
	int isLL1 = 1;

	for (itr = grammar.rules.head; itr != NULL; itr = itr->next)
	{
		int rowNo;
		Rule* rule = (Rule*)itr->element;

		//printf("\n\nConsidering Rule: "); printRule(rule); printf("\n");

		HashSet correspondingTerminals;

		HashSet firstSet = first(rule->rhsSymbols, grammar);

		//
		/*printf("First of %s = ", rule->lhs->symbolName);
		HashSetIterator itr = set_getIterator(firstSet);
		while (!set_iterationEnded(firstSet, itr))
		{
			Symbol* fi = (Symbol*)itr.bucketNode->element;
			printf("%s, ", fi->symbolName);
			itr = set_getNextIterator(firstSet, itr);
		}
		printf("	");*/
		//

		if (set_contains(firstSet, grammar.epsilon))
		{
			HashSet followSet = follow(rule->lhs, grammar);

			/*printf("Follow of %s = ", rule->lhs->symbolName);
			HashSetIterator itr = set_getIterator(followSet);
			while (!set_iterationEnded(followSet, itr))
			{
				Symbol* fo = (Symbol*)itr.bucketNode->element;
				printf("%s, ", fo->symbolName);
				itr = set_getNextIterator(followSet, itr);
			}
			printf("	");*/

			correspondingTerminals = set_union(firstSet, followSet);
			firstSet = set_clear(firstSet);
			followSet = set_clear(followSet);
		}
		else
		{
			correspondingTerminals = firstSet;
		}

		rowNo = rule->lhs->symbolID;
		
		for (columnIterator = set_getIterator(correspondingTerminals); !set_iterationEnded(correspondingTerminals, columnIterator); columnIterator = set_getNextIterator(correspondingTerminals, columnIterator))
		{
			int col = ((Symbol*)columnIterator.bucketNode->element)->symbolID;
			if (parseTable[rowNo][col] != NULL)
			{
				printf("\n\nGrammar Not LL1: Conflicting rules for (%d,%d):\n", rowNo, col);
				printRule(parseTable[rowNo][col]); printf("\n");
				printRule(rule);
				printf("\nThe latter rule will be retained for this cell\n");
				isLL1 = 0;
			}
			parseTable[rowNo][col] = rule;
		}
		correspondingTerminals = set_clear(correspondingTerminals);
	}

	if (isLL1) printf("\nThe grammar is LL1.. OK :)");
}

int compareRulesByReference(Rule* r1, Rule* r2)
{
	return r1 == r2;
}

Symbol* findSymbolByName(char* symbolString, Grammar g)
{
	if (symbolString[0] == '<')
	{
		Symbol s;
		strcpy(s.symbolName, symbolString);
		return (Symbol*)set_getElement(g.nonTerminalSet, &s);
	}
	else
	{
		Symbol s;
		strcpy(s.symbolName, symbolString);
		return (Symbol*)set_getElement(g.terminalsSet, &s);
	}
}

void printParseTable(Parser p)
{
	int row, col;
	printf("\n\n\nThe Parse Table:\n\n");
	for (row = 0; row < p.grammar.nonTerminalSet.hashTable.size; row++)
	{
		for (col = 0; col < p.grammar.terminalsSet.hashTable.size; col++)
		{
			if (p.parseTable[row][col])
			{
				printf("(%d, %d): ", row, col);
				printRule(p.parseTable[row][col]);
				printf("\n");
			}
		}
	}
}

TreeNode* findFirstNonTerminalLeafNode(TreeNode* t)
{
	ParseTreeNodeData* nodeData;
	if (t == NULL) return NULL;
	nodeData = (ParseTreeNodeData*)t->data;
	if (nodeData->symbol->isNonTerminal)
	{
		int i;
		if (t->no_of_children == 0) return t;
		for (i = 0; i < t->no_of_children; i++)
		{
			TreeNode* ans;
			ParseTreeNodeData* childData = (ParseTreeNodeData*)t->children[i]->data;
			if (!childData->symbol->isNonTerminal) continue;
			if (ans = findFirstNonTerminalLeafNode(t->children[i])) return ans;
		}
	}
	return NULL;
}

void _printParseTree(TreeNode* PT, FILE* outputFile)
{
	int i;
	ParseTreeNodeData *nodeData, *parentData;
	if (PT == NULL) return;
	
	for (i = 0; i < PT->no_of_children; i++)
	{
		_printParseTree(PT->children[i], outputFile);
	}

	nodeData = (ParseTreeNodeData*)PT->data;
	parentData = (ParseTreeNodeData*)PT->parent->data;

	//lexeme lineNo token value parent yes nodeSymbol
	
	if (nodeData->symbol->isNonTerminal)
	{
		//----	----	----	----	parent no	symbolName
		fprintf(outputFile, "\n  %-25s \t| %-7s \t| %-10s \t| %-8s \t| %-30s \t| %-12s \t| %-30s", "----"
			, "----"
			, "----"
			, "----"
			, parentData->symbol->symbolName
			, "no"
			, nodeData->symbol->symbolName);
	}
	else
	{	
		if (strcmp(nodeData->tokenInfo.token, "NUM") == 0 || strcmp(nodeData->tokenInfo.token, "RNUM") == 0)
		{

			//lexeme lineNo token value parent yes ---
			fprintf(outputFile, "\n  %-25s \t| %-7d \t| %-10s \t| %-8s \t| %-30s \t| %-12s \t| %-30s", nodeData->tokenInfo.lexeme
				, nodeData->tokenInfo.lineNo
				, nodeData->tokenInfo.token
				, nodeData->tokenInfo.lexeme
				, parentData->symbol->symbolName
				, "yes"
				, "----");
		}
		else
		{
			//lexeme lineNo token value parent yes ---
			fprintf(outputFile, "\n  %-25s \t| %-7d \t| %-10s \t| %-8s \t| %-30s \t| %-12s \t| %-30s", nodeData->tokenInfo.lexeme
				, nodeData->tokenInfo.lineNo
				, nodeData->tokenInfo.token
				, "----"
				, parentData->symbol->symbolName
				, "yes"
				, "----");
		}
	}
}

void printParseTree(Tree  PT, char *outfile)
{
	FILE* outputFile = fopen(outfile, "w");
	if (outputFile)
	{
		printf("\nPrinting Parse Tree...");

		fprintf(outputFile, "\n  %-25s \t| %-7s \t| %-10s \t| %-8s \t| %-30s \t| %-12s \t| %-30s"
			, "Lexeme"
			, "Line No"
			, "Token"
			, "Value"
			, "Parent"
			, "Is Leaf Node"
			, "Node Symbol");

		fprintf(outputFile, "\n----------------------------------------------------------------------------------------------");
		fprintf(outputFile, "----------------------------------------------------------------------------------------------\n");
		_printParseTree(PT.root->children[0], outputFile);

		printf("\nParse Tree printed to %s.", outfile);

		fclose(outputFile);
	}
	else
	{
		printf("\nError: Could not open file %s for writing parse tree. :(", outfile);
	}
}

void printParseTreeNatural(TreeLink subtreeRoot)
{
	int i;
	printf("\n\nMy Name is %s and I have %d children: ", ((ParseTreeNodeData*)subtreeRoot->data)->symbol->symbolName, subtreeRoot->no_of_children);
	
	for (i = 0; i < subtreeRoot->no_of_children; i++)
	{
		printf("	%s", ((ParseTreeNodeData*)subtreeRoot->children[i]->data)->symbol->symbolName);
	}
	for (i = 0; i < subtreeRoot->no_of_children; i++)
	{
		printParseTreeNatural(subtreeRoot->children[i]);
	}
}

void initUsefulArray(FILE* fin)
{
	int size = 1;
	while (fscanf(fin, "%s", usefullArray[size++]) > 0){}
}

UsefulNess getUsefullness(int ruleNo, int index)
{
	return (UsefulNess)(usefullArray[ruleNo][index] -'0');
}

int parseInputSourceCode(Parser* parser, Lexer l)
{
	Stack parserMainStack;
	Symbol* stackTop;
	TokenInfo lookAheadToken;
	Symbol* lookAheadSymbol;
	//stores pointers to those data of parse tree nodes for which tokenInfo is unknown because the lexer has not yet read those terminals from input.
	LinkList unInformedTerminalsInTree;
	//The node at which children are to be added when next time a rule is followed.
	TreeNode* currentTreeNode;
	Symbol* rootSymbol;
	Symbol* startSymbol;
	Rule* startRule;
	int usefullNessRHScount;
	ParseTreeNodeData* startSymbolNode;

	//Add a symbol "ROOT" as root of parse tree
	rootSymbol = (Symbol* )malloc(sizeof(Symbol));
	rootSymbol->isNonTerminal = 1;
	rootSymbol->symbolID = -1;
	strcpy(rootSymbol->symbolName, "ROOT");
	parser->parseTree = tree_add(parser->parseTree, NULL, parseTree_createNodeNTValue(rootSymbol));

	//Add the start symbol to the parse tree
	startRule = (Rule*)parser->grammar.rules.head->element;


	startSymbol = ((Rule*)parser->grammar.rules.head->element)->lhs;
	startSymbolNode = parseTree_createNodeNTValue(startSymbol);
	//modification..................................................................................................
	startSymbolNode->usefulNess = concrete;
	//......................................................................................
	parser->parseTree = tree_add(parser->parseTree, parser->parseTree.root, startSymbolNode);
	currentTreeNode = findFirstNonTerminalLeafNode(parser->parseTree.root);
	
	parserMainStack = stack_createNew(compareByReference);
	parserMainStack = stack_push(parserMainStack, parser->grammar.dollar);
	parserMainStack = stack_push(parserMainStack, ((Rule*)parser->grammar.rules.head->element)->lhs);

	unInformedTerminalsInTree = linkList_createNew(compareByReference);

	lookAheadToken = getNextToken(&l);

	printf("\nParsing Input... \n");

	while (1)
	{	
		
		if (strcmp(lookAheadToken.token, "BLANK") == 0 || strcmp(lookAheadToken.token, "COMMENT") == 0)
		{
			lookAheadToken = getNextToken(&l);
			continue;
		}
		if (strcmp(lookAheadToken.token, "BAD_TOKEN")==0)
		{
			printf("\nLine %d: Syntax Error: Bad Token: %s", lookAheadToken.lineNo, lookAheadToken.lexeme);
			parserMainStack = linkList_clear(parserMainStack);
			return 0;
		}

		
		lookAheadSymbol = findSymbolByName(lookAheadToken.token, parser->grammar);
		//lookAheadSymbol now points to one of the symbols in the set of grammar symbols.

		stackTop = (Symbol*)stack_top(parserMainStack);
		
		//printf("\n\nLookAhead is %s and StackTop is %s", lookAheadSymbol->symbolName, stackTop->symbolName);

		if (stackTop == parser->grammar.dollar || lookAheadSymbol == parser->grammar.dollar)
		{
			if (stackTop == lookAheadSymbol)
			{
				return 1;
			}
			else
			{
				if (stackTop == parser->grammar.dollar)
				{
					printf("\nLine %d: Syntax Error: The token %s for lexeme %s is unexpected. Expected end of file.", lookAheadToken.lineNo, lookAheadToken.token, lookAheadToken.lexeme);
				}
				else
				{
					printf("\nLine %d: Syntax Error: Unexpected end of file. ", lookAheadToken.lineNo);
					if (stackTop->isNonTerminal == 0)
					{
						printf(" The expected token is %s", stackTop->symbolName);
					}
				}
				return 0;
			}
		}
		else if (stackTop == parser->grammar.epsilon)
		{
			//printf("\nPopping Terminal %s", stackTop->symbolName);
			ParseTreeNodeData* terminalNodeValue;
			parserMainStack = stack_pop(parserMainStack);
			terminalNodeValue = (ParseTreeNodeData*)unInformedTerminalsInTree.head->element;
			strcpy(terminalNodeValue->tokenInfo.lexeme, "epsilon");
			terminalNodeValue->tokenInfo.lineNo = -1;
			strcpy(terminalNodeValue->tokenInfo.token, "----");
			unInformedTerminalsInTree = linkList_remove(unInformedTerminalsInTree, terminalNodeValue);
		}
		else if (stackTop->isNonTerminal == 0 && stackTop == lookAheadSymbol)
		{
			//printf("\nPopping Terminal %s", stackTop->symbolName);
			ParseTreeNodeData* terminalNodeValue = (ParseTreeNodeData*)unInformedTerminalsInTree.head->element;
			terminalNodeValue->tokenInfo = lookAheadToken;
			unInformedTerminalsInTree = linkList_remove(unInformedTerminalsInTree, terminalNodeValue);
			parserMainStack = stack_pop(parserMainStack);
			lookAheadToken = getNextToken(&l);
		}
		else if (stackTop->isNonTerminal && parser->parseTable[stackTop->symbolID][lookAheadSymbol->symbolID]!=NULL)
		{
			Stack stackTemp;
			LinkListNode* rhsIterator;
			LinkList tempLinkList = linkList_createNew(compareByReference);
			Rule* thisRule = parser->parseTable[stackTop->symbolID][lookAheadSymbol->symbolID];
			ParseTreeNodeData* currentTreeNodeData = (ParseTreeNodeData*)currentTreeNode->data;
			usefullNessRHScount = 0;
			currentTreeNodeData->expandsToRule = thisRule;
			//printf("\nFollowing Rule: "); printRule(thisRule);
			stackTemp = linkList_createNew(compareByReference); //RHS symbols are first poured into this stack and then into mainStack
																	//so that they are poured in correct order.
			

			parserMainStack = stack_pop(parserMainStack);
			
			for (rhsIterator = thisRule->rhsSymbols.head; rhsIterator != NULL; rhsIterator = rhsIterator->next)
			{	
				Symbol* thisSymbol = (Symbol*)rhsIterator->element;
				stackTemp = stack_push(stackTemp, rhsIterator->element);

				//Add the RHS symbols to parse tree.
				if (thisSymbol->isNonTerminal)
				{
					ParseTreeNodeData* childNodeData = parseTree_createNodeNTValue(thisSymbol);
					childNodeData->expandsFrom = thisRule;
					childNodeData->usefulNess = getUsefullness(thisRule->ruleNo, usefullNessRHScount++);
					parser->parseTree = tree_add(parser->parseTree, currentTreeNode, childNodeData);
				}
				else
				{	
					//this is actually insertion of terminal only with garbage token info for time being....would be corrected later.
					ParseTreeNodeData* terminalTreeNodeValue = parseTree_createNodeNTValue(thisSymbol);
					terminalTreeNodeValue->expandsFrom = thisRule;
					terminalTreeNodeValue->usefulNess = getUsefullness(thisRule->ruleNo, usefullNessRHScount++);
					parser->parseTree = tree_add(parser->parseTree, currentTreeNode,terminalTreeNodeValue );
					tempLinkList = linkList_addToTail(tempLinkList, terminalTreeNodeValue);
				}
			}

			unInformedTerminalsInTree = linkList_merge(tempLinkList, unInformedTerminalsInTree);

			currentTreeNode = findFirstNonTerminalLeafNode(parser->parseTree.root); //find the next currentNode
			

			while (stackTemp.size != 0)
			{
				parserMainStack = stack_push(parserMainStack, stack_top(stackTemp));
				stackTemp = stack_pop(stackTemp);
			}
		}
		else
		{
			printf("\nLine %d: Syntax Error: The token %s for lexeme %s is unexpected.", lookAheadToken.lineNo, lookAheadToken.token, lookAheadToken.lexeme);
			if (stackTop->isNonTerminal == 0)
			{
				printf(" The expected token here is %s", stackTop->symbolName);
			}
			parserMainStack = linkList_clear(parserMainStack);
			return 0;
		}
	}
	return 0;
}

ParseTreeNodeData* parseTree_createNodeValue(Symbol* symbol, TokenInfo info)
{
	ParseTreeNodeData* ptNodeInfo = (ParseTreeNodeData*)malloc(sizeof(ParseTreeNodeData));
	ptNodeInfo->symbol = symbol;
	ptNodeInfo->tokenInfo = info;
	ptNodeInfo->expandsFrom = NULL;
	ptNodeInfo->expandsToRule = NULL;
	return ptNodeInfo;
}

ParseTreeNodeData* parseTree_createNodeNTValue(Symbol* symbol)
{
	ParseTreeNodeData* ptNodeInfo = (ParseTreeNodeData*)malloc(sizeof(ParseTreeNodeData));
	ptNodeInfo->symbol = symbol;
	ptNodeInfo->expandsFrom = NULL;
	ptNodeInfo->expandsToRule = NULL;
	ptNodeInfo->tokenInfo.lexeme[0] = '\0';
	ptNodeInfo->tokenInfo.lineNo = -1;
	ptNodeInfo->tokenInfo.token[0] = '\0';
	return ptNodeInfo;
}
