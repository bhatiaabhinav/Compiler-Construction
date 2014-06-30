/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#ifndef _LEXER_
#define _LEXER_

#include "lexerDef.h"

FILE *getStream(FILE *fp, Buffer B, Buffersize k);

TokenInfo  getNextToken(Lexer* pLexer);

Lexer lexer_createNew(FILE* pSourceFile, FILE* keyWordsFile);

HashTable readKeywordsIntoHashTable(FILE* keyWordsFile);

int compareTokenInfoByLexeme(TokenInfo* t1, TokenInfo* t2);

int hashTokenInfoByLexeme(TokenInfo* t, int hashTableSize);

void lexer_runLexicalAnalyses(Lexer lex);

#endif