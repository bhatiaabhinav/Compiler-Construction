/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#include "follow.h"
#include "compiler.h"
#include "first.h"
#include "definitions.h"


LinkList findRulesInWhichItComes(Symbol* NT, Grammar grammar)
{
	LinkList ans = linkList_createNew(compareByReference);
	LinkListNode* itr;
	for (itr = grammar.rules.head; itr != NULL; itr = itr->next)
	{
		Rule* r = (Rule*)itr->element;
		if (linkList_find(r->rhsSymbols, NT))
		{
			ans = linkList_add(ans, r);
		}
	}
	return ans;
}

//followCalled is an array which stores whether Follow of the corresponding symbol had already been called in past
HashSet _follow(Symbol* NT, Grammar g, int* followCalled)
{
	followCalled[NT->symbolID] = 1;

	HashSet ans = set_create(g.terminalsSet.hashTable.size, compareByReference, hashFunctionForSymbols);

	LinkList goodRules = findRulesInWhichItComes(NT, g);

	LinkListNode* ruleItr;

	for (ruleItr = goodRules.head; ruleItr != NULL; ruleItr = ruleItr->next)
	{
		Rule* r = (Rule*)ruleItr->element;
		LinkListNode* NTlocationInRHS = linkList_find(r->rhsSymbols, NT);
		if (NTlocationInRHS->next == NULL)
		{
			if (!followCalled[r->lhs->symbolID])
			{
				HashSet followOfLHS = _follow(r->lhs, g, followCalled);
				HashSet unionedAns = set_union(ans, followOfLHS);
				ans = set_clear(ans);
				followOfLHS = set_clear(followOfLHS);
				ans = unionedAns;
			}
		}
		else
		{
			LinkList trailingSymbols = linkList_createNew(compareByReference);
			LinkListNode* itr;
			for (itr = NTlocationInRHS->next; itr != NULL; itr = itr->next)
			{
				trailingSymbols = linkList_addToTail(trailingSymbols, itr->element);
			}

			HashSet firstOfTrailingSymbols = first(trailingSymbols, g);

			if (set_contains(firstOfTrailingSymbols, g.epsilon))
			{
				firstOfTrailingSymbols = set_remove(firstOfTrailingSymbols, g.epsilon);
				HashSet unionedAns = set_union(ans, firstOfTrailingSymbols);
				ans = set_clear(ans);
				firstOfTrailingSymbols = set_clear(firstOfTrailingSymbols);
				ans = unionedAns;
				if (!followCalled[r->lhs->symbolID])
				{
					HashSet followOfLHS = _follow(r->lhs, g, followCalled);
					unionedAns = set_union(ans, followOfLHS);
					ans = set_clear(ans);
					followOfLHS = set_clear(followOfLHS);
					ans = unionedAns;
				}
			}
			else
			{
				HashSet unionedAns = set_union(ans, firstOfTrailingSymbols);
				ans = set_clear(ans);
				firstOfTrailingSymbols = set_clear(firstOfTrailingSymbols);
				ans = unionedAns;
			}

			trailingSymbols = linkList_clear(trailingSymbols);
		}
	}

	goodRules = linkList_clear(goodRules);

	return ans;
}

HashSet follow(Symbol* nonTerminal, Grammar g)
{
	//followCalled is an array which stores whether Follow of the corresponding symbol had already been called in past
	int* followCalled = (int*)malloc(g.nonTerminalSet.hashTable.size* sizeof(int));

	int i;
	for (i = 0; i < g.nonTerminalSet.hashTable.size; i++)
	{
		followCalled[i] = 0;
	}

	HashSet ans = _follow(nonTerminal, g, followCalled);
	free(followCalled);

	return ans;
}


