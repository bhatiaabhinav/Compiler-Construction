/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#include "first.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "set.h"
#include "parser.h"
#include "compiler.h"

//Returns a list of rules for which LHS is symbol. Compares symbols by reference
LinkList getAllStartRules(Symbol* symbol, Grammar grammar)
{
	LinkList allStartRules = linkList_createNew(compareByReference);
	LinkListNode* itr;
	for (itr = grammar.rules.head; itr != NULL; itr = itr->next)
	{
		Symbol* left = ((Rule*)(itr->element))->lhs;
		if (left == symbol)
		{
			allStartRules = linkList_add(allStartRules, itr->element);
		}
	}

	return allStartRules;
}




HashSet first(LinkList listOfSymbols, Grammar grammar)
{
	HashSet ans = set_create(MAX_NO_SYMBOLS, compareByReference, hashFunctionForSymbols);
	Symbol* firstToken;
	LinkList restOfFirst = linkList_createNew(compareByReference);
	HashSet UnionedfirstOfFirst;
	HashSet firstOfFirst;
	LinkList singleTonList;

	firstToken = (Symbol*)listOfSymbols.head->element;

	if (firstToken->isNonTerminal == 0)
	{
		ans = set_add(ans, firstToken);
		return ans;
	}

	LinkList startRules;
	startRules = linkList_createNew(compareByReference);
	LinkListNode* itr;

	if (listOfSymbols.size == 1)
	{
		startRules = getAllStartRules(firstToken, grammar);
		itr = startRules.head;
		for (; itr != NULL; itr = itr->next)
		{
			LinkList rhs = ((Rule*)itr->element)->rhsSymbols;
			HashSet rhsFirst = first(rhs, grammar);
			HashSet Unionedans = set_union(ans, rhsFirst);
			ans = set_clear(ans);
			rhsFirst = set_clear(rhsFirst);
			ans = Unionedans;
		}

		startRules = linkList_clear(startRules);
		int a = 4;
		int b = a + 5;
		return ans;
	}

	singleTonList = linkList_createNew(compareByReference);
	singleTonList = linkList_add(singleTonList, listOfSymbols.head->element);

	firstOfFirst = first(singleTonList, grammar);

	if (!set_contains(firstOfFirst, grammar.epsilon))
	{
		return firstOfFirst;
	}

	else
	{
		firstOfFirst = set_remove(firstOfFirst, grammar.epsilon);
		restOfFirst.head = listOfSymbols.head->next;
		restOfFirst.size = listOfSymbols.size - 1;

		UnionedfirstOfFirst = set_union(firstOfFirst, first(restOfFirst, grammar));
		set_clear(firstOfFirst);

		free(singleTonList.head);
		startRules = linkList_clear(startRules);
		return UnionedfirstOfFirst;
	}
}