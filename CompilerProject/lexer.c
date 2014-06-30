/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#include "lexer.h"
#include "commons.h"

FILE* getStream(FILE *fp, Buffer B, Buffersize k)
{
	int n;
	if (fp == NULL) return fp;
	n = fread(B, 1, BUFFER_SIZE, fp);
	if (n != BUFFER_SIZE)
	{
		fclose(fp);
		fp = NULL;
		B[n] = EOF;
	}
	return fp;
}

TokenInfo tokenize(Buffer lexemeBuffer, int* pLexemeBufferIndex, int backTrackAmount, char* tokenizeAs, int lineNo)
{
	int i;
	TokenInfo newToken;
	int temp;
	int min = *pLexemeBufferIndex - backTrackAmount < MAX_LEXEME_SIZE - 1 ? *pLexemeBufferIndex - backTrackAmount : MAX_LEXEME_SIZE - 1;
	for (i = 0; i < min; i++) newToken.lexeme[i] = lexemeBuffer[i];
	newToken.lexeme[i] = '\0';
	strcpy(newToken.token, tokenizeAs);

	//backtracking:
	temp = *pLexemeBufferIndex - (backTrackAmount);
	for (i = 0; i < backTrackAmount; i++)
	{
		lexemeBuffer[i] = lexemeBuffer[temp];
		temp++;
	}
	newToken.lineNo = lineNo;

	*pLexemeBufferIndex = backTrackAmount;
	lexemeBuffer[*pLexemeBufferIndex] = '\0';

	return newToken;
}

TokenInfo takeAction(Buffer lexemeBuffer, int* pLexemeBufferIndex, char* actionName, HashTable keywordsTable, int lineNo)
{
	if (actionName[0] != '#')
	{
		return tokenize(lexemeBuffer, pLexemeBufferIndex, 0, actionName, lineNo);
	}
	else
	{
		switch (actionName[1])
		{
		case 'E': //#ERROR
			lexemeBuffer[*pLexemeBufferIndex] = '\0';
			printf("\nLexer Error: Unknown Pattern %s at line %d", lexemeBuffer, lineNo);
			return tokenize(lexemeBuffer, pLexemeBufferIndex, 0, "BAD_TOKEN", lineNo);
		case 'B':
		{
					int backTrackAmt = (int)(actionName[6] - '0');
					char token[256]; int i = 8;
					while (1)
					{
						token[i - 8] = actionName[i];
						if (actionName[i] == 0) break;
						i++;
					}
					return tokenize(lexemeBuffer, pLexemeBufferIndex, backTrackAmt, token, lineNo);
		}
		case 'C': //"#CHECK_KEYWORD_ID" or "#CHECK_LEN_ID"
			if (actionName[7] == 'L') //#CHECK_LEN_ID
			{
				if (*pLexemeBufferIndex <= 20) return tokenize(lexemeBuffer, pLexemeBufferIndex, 0, "ID", lineNo);
				else
				{
					printf("Identifier at line %d is longer than the prescribed length of 20 characters", lineNo);
					return tokenize(lexemeBuffer, pLexemeBufferIndex, 0, "BAD_TOKEN", lineNo);
				}
			}
			else
			{
				TokenInfo newToken; int i;
				TokenInfo* keywordToken;
				for (i = 0; i < *pLexemeBufferIndex; i++) newToken.lexeme[i] = lexemeBuffer[i];
				newToken.lexeme[i] = '\0';
				keywordToken = (TokenInfo*)hashTable_getElement(keywordsTable, &newToken);
				if (keywordToken)
				{
					strcpy(newToken.token, keywordToken->token);
					*pLexemeBufferIndex = 0;
					lexemeBuffer[*pLexemeBufferIndex] = '\0';
					newToken.lineNo = lineNo;
					return newToken;
				}
				else
				{
					return takeAction(lexemeBuffer, pLexemeBufferIndex, "#CHECK_LEN_ID", keywordsTable, lineNo);
				}
				return tokenize(lexemeBuffer, pLexemeBufferIndex, 0, "ID", lineNo); //for now. To be modified after keyword lookup table is built
			}
		default:
			return tokenize(lexemeBuffer, pLexemeBufferIndex, 0, "BAD_TOKEN", lineNo);

		}
	}
}

TokenInfo getNextToken(Lexer* pLexer)
{
	int i;
	pLexer->moreTokensPossible = 1;

	dfa_reset(pLexer->pDFA);

	
	for (i = 0; i < pLexer->lexemeBufferIndex; i++)
	{
		dfa_makeNextTransition(pLexer->pDFA, pLexer->lexemeBuffer[i]);
		if (pLexer->pDFA->states[pLexer->pDFA->currentStateID]->hasAction) goto tokenise;
	}


	while (1)
	{
		char c;
		if (pLexer->mainBufferIndex == BUFFER_SIZE)
		{
			pLexer->pSourceFile = getStream(pLexer->pSourceFile, pLexer->currentBuffer, BUFFER_SIZE);
			if (pLexer->currentBuffer == pLexer->upperBuffer) pLexer->currentBuffer = pLexer->lowerBuffer;
			else if (pLexer->currentBuffer == pLexer->lowerBuffer) pLexer->currentBuffer = pLexer->upperBuffer;
			pLexer->mainBufferIndex = 0;
		}


		c = (pLexer->currentBuffer)[pLexer->mainBufferIndex];
		if (c == EOF)
		{
			pLexer->moreTokensPossible = 0;
		}
		dfa_makeNextTransition(pLexer->pDFA, c);
		if (pLexer->pDFA->states[pLexer->pDFA->currentStateID]->hasAction) break;

		pLexer->lexemeBuffer[(pLexer->lexemeBufferIndex)++] = c;
		(pLexer->mainBufferIndex)++;
		if (c == '\n') pLexer->lineNo++;
	}

	tokenise:

	return takeAction(pLexer->lexemeBuffer, &(pLexer->lexemeBufferIndex), pLexer->pDFA->states[pLexer->pDFA->currentStateID]->actionName, pLexer->keyWordTable, pLexer->lineNo); //backTrackAmount gives characters finally remaining
}

int condition_a(char c){ return c == 'a'; }
int condition_o(char c){ return c == 'o'; }
int condition_n(char c){ return c == 'n'; }
int condition_d(char c){ return c == 'd'; }
int condition_r(char c){ return c == 'r'; }
int condition_t(char c){ return c == 't'; }
int condition_m(char c){ return c == 'm'; }
int condition_i(char c){ return c == 'i'; }
int condition_period(char c) { return c == '.'; }
int condiiton_lessThan(char c) { return c == '<'; }
int condition_greaterThan(char c){ return c == '>'; }
int condition_equals(char c){ return c == '='; }
int condition_forwardSlash(char c){ return c == '/'; }
int condition_doubleQuote(char c){ return c == '\"'; }
int condition_squareBracketOpen(char c){ return c == '['; }
int condition_squareBracketClose(char c){ return c == ']'; }
int condition_parenOpen(char c){ return c == '('; }
int condition_parenClose(char c){ return c == ')'; }
int condition_underScore(char c){ return c == '_'; }
int condition_semicolon(char c){ return c == ';'; }
int condition_comma(char c){ return c == ','; }
int condition_plus(char c){ return c == '+'; }
int condition_hyphen(char c){ return c == '-'; }
int condition_asterisk(char c){ return c == '*'; }
int condition_atTheRate(char c){ return c == '@'; }
int condition_hash(char c){ return c == '#'; }
int condition_EOF(char c){ return c == EOF; }
int condition_numeric(char c){ return (('0' <= c) && (c <= '9')); }
int condition_alphabet(char c) { return (((c >= 'a') && (c <= 'z')) || ((c >= 'A'&&c <= 'Z'))); }
int condition_alphabetSmall(char c){ return (c >= 'a') && (c <= 'z'); }
int condition_alphabetSmallOrSpace(char c){ return c == ' ' || condition_alphabetSmall(c); }
int condition_alphanumeric(char c){ return condition_alphabet(c) || condition_numeric(c); }
int condition_alphabetExcept_m(char c){ return condition_alphabet(c) && !condition_m(c); }
int condition_alphanumericExcept_a(char c) { return !condition_a(c) && condition_alphanumeric(c); }
int condition_alphanumericExcept_i(char c){ return !condition_i(c) && condition_alphanumeric(c); }
int condition_alphanumericExcept_n(char c){ return !condition_n(c) && condition_alphanumeric(c); }
int condition_anythingExceptNewLine(char c) { return c != '\n' && c != '\r' && c != EOF; }
int condition_blank(char c){ return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }


void populateDFA(DFA* dfa)
{
	int numberOfNonActionStates = 53;
	DFAState** nonActionStates = (DFAState**)malloc(numberOfNonActionStates * sizeof(DFAState*));

	char* actionNames[] = { "#ERROR", "NUM", "#BACK_1_NUM", "RNUM", "#CHECK_KEYWORD_ID", "#CHECK_LEN_ID", "AND", "OR", //0 to 7
		"NOT", "STR", "FUNID", "MAIN", "LT", "LE", "GT", "GE", "ASSIGNOP", "EQ", "#BACK_1_ASSIGNOP", "NE", //8 to 19
		"SQO", "SQC", "OP", "CL", "SEMICOLON", "COMMA", "PLUS", "MINUS", "MUL", "DIV", "SIZE", "COMMENT", "BLANK", "$" }; //20 to 33

	int numberOfActionStates = 34;

	DFAState** actionStates = (DFAState**)malloc(numberOfActionStates * sizeof(DFAState*));

	int i;
	for (i = 0; i < numberOfNonActionStates; i++)
	{
		nonActionStates[i] = dfa_createState(0, ""); dfa_addState(dfa, nonActionStates[i]);
	}

	dfa_addTransition(dfa, 0, condition_numeric, 1);
	dfa_addTransition(dfa, 1, condition_numeric, 1);
	dfa_addTransition(dfa, 1, condition_period, 2);
	dfa_addTransition(dfa, 2, condition_numeric, 3);
	dfa_addTransition(dfa, 3, condition_numeric, 4);
	dfa_addTransition(dfa, 0, condition_period, 5);
	dfa_addTransition(dfa, 5, condition_a, 8);
	dfa_addTransition(dfa, 8, condition_n, 9);
	dfa_addTransition(dfa, 9, condition_d, 10);
	dfa_addTransition(dfa, 10, condition_period, 11);
	dfa_addTransition(dfa, 5, condition_o, 12);
	dfa_addTransition(dfa, 12, condition_r, 13);
	dfa_addTransition(dfa, 13, condition_period, 14);
	dfa_addTransition(dfa, 5, condition_n, 15);
	dfa_addTransition(dfa, 15, condition_o, 16);
	dfa_addTransition(dfa, 16, condition_t, 17);
	dfa_addTransition(dfa, 17, condition_period, 18);
	dfa_addTransition(dfa, 0, condition_alphabet, 6);
	dfa_addTransition(dfa, 6, condition_alphabet, 6);
	dfa_addTransition(dfa, 6, condition_numeric, 7);
	dfa_addTransition(dfa, 0, condition_doubleQuote, 19);
	dfa_addTransition(dfa, 19, condition_alphabetSmallOrSpace, 20);
	dfa_addTransition(dfa, 20, condition_alphabetSmallOrSpace, 20);
	dfa_addTransition(dfa, 20, condition_doubleQuote, 21);
	dfa_addTransition(dfa, 0, condition_underScore, 22);
	dfa_addTransition(dfa, 22, condition_alphabetExcept_m, 23);
	dfa_addTransition(dfa, 23, condition_alphanumeric, 23);
	dfa_addTransition(dfa, 22, condition_m, 24);
	dfa_addTransition(dfa, 24, condition_alphanumericExcept_a, 23);
	dfa_addTransition(dfa, 24, condition_a, 25);
	dfa_addTransition(dfa, 25, condition_alphanumericExcept_i, 23);
	dfa_addTransition(dfa, 25, condition_i, 26);
	dfa_addTransition(dfa, 26, condition_alphanumericExcept_n, 23);
	dfa_addTransition(dfa, 26, condition_n, 27);
	dfa_addTransition(dfa, 27, condition_alphanumeric, 23);
	dfa_addTransition(dfa, 0, condiiton_lessThan, 28);
	dfa_addTransition(dfa, 28, condition_equals, 29);
	dfa_addTransition(dfa, 0, condition_greaterThan, 30);
	dfa_addTransition(dfa, 30, condition_equals, 31);
	dfa_addTransition(dfa, 0, condition_equals, 32);
	dfa_addTransition(dfa, 32, condition_equals, 34);
	dfa_addTransition(dfa, 32, condition_forwardSlash, 33);
	dfa_addTransition(dfa, 33, condition_equals, 35);
	dfa_addTransition(dfa, 0, condition_squareBracketOpen, 36);
	dfa_addTransition(dfa, 0, condition_squareBracketClose, 37);
	dfa_addTransition(dfa, 0, condition_parenOpen, 38);
	dfa_addTransition(dfa, 0, condition_parenClose, 39);
	dfa_addTransition(dfa, 0, condition_semicolon, 40);
	dfa_addTransition(dfa, 0, condition_comma, 41);
	dfa_addTransition(dfa, 0, condition_plus, 42);
	dfa_addTransition(dfa, 0, condition_hyphen, 43);
	dfa_addTransition(dfa, 0, condition_asterisk, 44);
	dfa_addTransition(dfa, 0, condition_forwardSlash, 45);
	dfa_addTransition(dfa, 0, condition_atTheRate, 46);
	dfa_addTransition(dfa, 0, condition_hash, 47);
	dfa_addTransition(dfa, 47, condition_anythingExceptNewLine, 48);
	dfa_addTransition(dfa, 48, condition_anythingExceptNewLine, 48);
	dfa_addTransition(dfa, 0, condition_blank, 49);
	dfa_addTransition(dfa, 49, condition_blank, 49);
	dfa_addTransition(dfa, 0, condition_EOF, 51);
	dfa_addTransition(dfa, 7, condition_numeric, 52);
	dfa_addTransition(dfa, 52, condition_numeric, 52);


	

	for (i = 0; i < numberOfActionStates; i++)
	{
		actionStates[i] = dfa_createState(1, actionNames[i]); dfa_addState(dfa, actionStates[i]);
	}

	dfa->states[0]->defaultToStateID = 50;
	dfa->states[1]->defaultToStateID = actionStates[1]->ID;
	dfa->states[2]->defaultToStateID = actionStates[2]->ID;
	dfa->states[3]->defaultToStateID = actionStates[0]->ID;
	dfa->states[4]->defaultToStateID = actionStates[3]->ID;
	dfa->states[5]->defaultToStateID = actionStates[0]->ID;
	dfa->states[6]->defaultToStateID = actionStates[4]->ID;
	dfa->states[7]->defaultToStateID = actionStates[5]->ID;
	dfa->states[8]->defaultToStateID = actionStates[0]->ID;
	dfa->states[9]->defaultToStateID = actionStates[0]->ID;
	dfa->states[10]->defaultToStateID = actionStates[0]->ID;
	dfa->states[11]->defaultToStateID = actionStates[6]->ID;
	dfa->states[12]->defaultToStateID = actionStates[0]->ID;
	dfa->states[13]->defaultToStateID = actionStates[0]->ID;
	dfa->states[14]->defaultToStateID = actionStates[7]->ID;
	dfa->states[15]->defaultToStateID = actionStates[0]->ID;
	dfa->states[16]->defaultToStateID = actionStates[0]->ID;
	dfa->states[17]->defaultToStateID = actionStates[0]->ID;
	dfa->states[18]->defaultToStateID = actionStates[8]->ID;
	dfa->states[19]->defaultToStateID = actionStates[0]->ID;
	dfa->states[20]->defaultToStateID = actionStates[0]->ID;
	dfa->states[21]->defaultToStateID = actionStates[9]->ID;
	dfa->states[22]->defaultToStateID = actionStates[0]->ID;
	dfa->states[23]->defaultToStateID = actionStates[10]->ID;
	dfa->states[24]->defaultToStateID = actionStates[10]->ID;
	dfa->states[25]->defaultToStateID = actionStates[10]->ID;
	dfa->states[26]->defaultToStateID = actionStates[10]->ID;
	dfa->states[27]->defaultToStateID = actionStates[11]->ID;
	dfa->states[28]->defaultToStateID = actionStates[12]->ID;
	dfa->states[29]->defaultToStateID = actionStates[13]->ID;
	dfa->states[30]->defaultToStateID = actionStates[14]->ID;
	dfa->states[31]->defaultToStateID = actionStates[15]->ID;
	dfa->states[32]->defaultToStateID = actionStates[16]->ID;
	dfa->states[33]->defaultToStateID = actionStates[18]->ID;
	dfa->states[34]->defaultToStateID = actionStates[17]->ID;
	dfa->states[35]->defaultToStateID = actionStates[19]->ID;
	dfa->states[36]->defaultToStateID = actionStates[20]->ID;
	dfa->states[37]->defaultToStateID = actionStates[21]->ID;
	dfa->states[38]->defaultToStateID = actionStates[22]->ID;
	dfa->states[39]->defaultToStateID = actionStates[23]->ID;
	dfa->states[40]->defaultToStateID = actionStates[24]->ID;
	dfa->states[41]->defaultToStateID = actionStates[25]->ID;
	dfa->states[42]->defaultToStateID = actionStates[26]->ID;
	dfa->states[43]->defaultToStateID = actionStates[27]->ID;
	dfa->states[44]->defaultToStateID = actionStates[28]->ID;
	dfa->states[45]->defaultToStateID = actionStates[29]->ID;
	dfa->states[46]->defaultToStateID = actionStates[30]->ID;
	dfa->states[47]->defaultToStateID = actionStates[31]->ID;
	dfa->states[48]->defaultToStateID = actionStates[31]->ID;
	dfa->states[49]->defaultToStateID = actionStates[32]->ID;
	dfa->states[50]->defaultToStateID = actionStates[0]->ID;
	dfa->states[51]->defaultToStateID = actionStates[33]->ID;
	dfa->states[52]->defaultToStateID = actionStates[0]->ID;

	dfa->startStateID = 0;
}

Lexer lexer_createNew(FILE* pSourceFile, FILE* keywordsFile)
{
	Lexer l;

	l.pSourceFile = pSourceFile;
	l.pDFA = dfa_createDFA();
	l.upperBuffer = (Buffer)malloc(BUFFER_SIZE * sizeof(char));
	l.lowerBuffer = (Buffer)malloc(BUFFER_SIZE * sizeof(char));
	l.currentBuffer = l.upperBuffer;
	l.lexemeBuffer = (Buffer)malloc(MAX_LINE_SIZE * sizeof(char));

	l.pSourceFile = getStream(pSourceFile, l.upperBuffer, BUFFER_SIZE);
	if (l.pSourceFile) pSourceFile = getStream(pSourceFile, l.lowerBuffer, BUFFER_SIZE);

	l.lineNo = 1;
	l.moreTokensPossible = 1;
	l.mainBufferIndex = 0;
	l.lexemeBufferIndex = 0;

	populateDFA(l.pDFA);
	l.keyWordTable = readKeywordsIntoHashTable(keywordsFile);
	return l;
}

int compareTokenInfoByLexeme(TokenInfo* t1, TokenInfo* t2)
{
	return  ((strcmp(t1->lexeme, t2->lexeme) == 0));
}

int hashTokenInfoByLexeme(TokenInfo* t, int hashTableSize)
{
	int sum = 0;
	int i = 0;
	while (t->lexeme[i])
	{
		sum += (int)(t->lexeme[i]);
		i++;
	}
	return sum % hashTableSize;
}

HashTable readKeywordsIntoHashTable(FILE* keyWordFile)
{
	TokenInfo* newToken;
	char lexeme[100];
	char token[20];
	HashTable keywordTable = hashTable_createNew(19, (int(*)(void*, void*))compareTokenInfoByLexeme, (int(*) (void* value, int hashTableSize)) hashTokenInfoByLexeme);
	while (fscanf(keyWordFile, "%s%s", lexeme, token) > 0)
	{
		newToken = (TokenInfo*)malloc(sizeof(TokenInfo));
		strcpy(newToken->lexeme, lexeme);
		strcpy(newToken->token, token);
		keywordTable = hashTable_add(keywordTable, newToken);
	}
	
	fclose(keyWordFile);
	return keywordTable;
}

void lexer_runLexicalAnalyses(Lexer lex)
{
	int errorCount = 0;
	int tokenCount = 0;
	printf("\n\nBeginning Full Lexical Analysis...\n");

	


	while (lex.moreTokensPossible)
	{
		TokenInfo t = getNextToken(&lex);
		if (!strcmp(t.token, "BAD_TOKEN")) errorCount++;
		else tokenCount++;

		if (strcmp(t.token, "BLANK") == 0 || strcmp(t.token, "COMMENT") == 0) continue;
		printf("\n  %-30s\t%-15s\t%-4d", t.lexeme, t.token, t.lineNo);
	}

	printf("\nLexical Analysis Finished. %d tokens. %d errors.\n\n", tokenCount, errorCount);
}