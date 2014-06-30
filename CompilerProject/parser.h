/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _PARSER_
#define _PARSER_

#include "parserDef.h"
#include "lexer.h"

Parser parser_initialise(FILE* grammarInputFile, FILE* astRulesFile);

Grammar populateRules(FILE *fp);

//Call this function after memory for parseTable has been allocated
void createParseTable(Grammar grammar, ParseTable parseTable);

int compareRulesByReference(Rule* r1, Rule* r2);

Symbol* findSymbolByName(char* symbolString, Grammar g);

//Returns success of parsing attempt.
int parseInputSourceCode(Parser* p, Lexer l);

void printParseTable(Parser p);

void printParseTree(Tree  PT, char *outfile);
void printParseTreeNatural(TreeLink subtreeRoot);

ParseTreeNodeData* parseTree_createNodeValue(Symbol* symbol, TokenInfo info);
ParseTreeNodeData* parseTree_createNodeNTValue(Symbol* symbol);

void initUsefulArray(FILE* fin);

#endif