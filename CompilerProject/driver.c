/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#include "compiler.h"
#include "first.h"
#include "follow.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "AST.h"
#include "SymbolTable.h"
#include "typeChecker.h"
#include "codeGenerator.h"

#define GRAMMAR_INPUT_FILENAME "realGrammar.txt"
#define KEYWORDS_INPUT_FILENAME "keywords.txt"
#define DEFAULT_OUTPUT_PARSE_TREE_FILE "parseTreeOutFile.txt"
#define AST_RULES_FILENAME "astRules.txt"

#define PARSER_NOT_INVOKED 0
#define PARSING_FAILED 1
#define PARSED_SUCCESSFULLY 2

int parseOnlySourceCode(Parser* p, Lexer lex)
{
	int result = parseInputSourceCode(p, lex);
	if (result)
	{
		printf("\n\nThe input is syntactically correct.  :)\n\n");
		return PARSED_SUCCESSFULLY;
	}
	else
	{
		printf("\n\nThe input is NOT syntactically correct.  :(\n\n");
		return PARSING_FAILED;
	}
}

int printOnlyParsetree(Parser* p, Lexer lex, char* outPutTreeFile, int isParsed)
{
	if (isParsed==PARSER_NOT_INVOKED)
	{
		isParsed = parseOnlySourceCode(p, lex);
	}

	if (isParsed == PARSED_SUCCESSFULLY)
	{
		printParseTree(p->parseTree, outPutTreeFile);
		return isParsed;
	}
	else
	{
		return isParsed;
	}
}



int main(int argc, char** argv)
{

	////////////////Basic Printing/////////////////////////
	//printf("Submitted by Batch No. 06:\n%30s\t(%s)\n%30s\t(%s)\n\n\n", "Abhinav Bhatia", "2011A7PS371P", "Mukul Bhutani ", "2011A7PS343P");
	
	printf("LEVEL %d: %s", 3, "AST/Symbol Table/Type checking/Symantic Rules modules work\n");
	printf("Code generation implemented for most constructs\n\n");
	////////////////Basic Printing End/////////////////////

	FILE* pSourceFile = NULL;
	int choice;
	int isParsed = PARSER_NOT_INVOKED;


	if (argc > 1)
	{
		pSourceFile = fopen(argv[1], "r");
	}
	else if (argc == 1)
	{
		printf("\nLess number of arguments ...Two arguments required....\n");
		printf("1. Source Code File.\n");
		printf("2. File for printing parse tree.\n");

		char c;
		scanf("%c", &c);
		return 0;
	}

	if (pSourceFile == NULL)
	{
		printf("\nError: No Input File!\nExiting...........");
		char c;
		scanf("%c", &c);
		return -1;
	}

	FILE* pKeywordsFile = fopen(KEYWORDS_INPUT_FILENAME, "r");
	if (!pKeywordsFile)
	{
		printf("\nError: keywords file \"%s\" not found.\nExiting..........", KEYWORDS_INPUT_FILENAME);
		char c;
		scanf("%c", &c);
		return -1;
	}
	FILE* grammarInputFile = fopen(GRAMMAR_INPUT_FILENAME, "r");
	if (!grammarInputFile)
	{
		printf("\nError: Grammar file \"%s\" not found.\nExiting..........", GRAMMAR_INPUT_FILENAME);
		char c;
		scanf("%c", &c);
		return -1;
	}
	FILE* astRulesFile = fopen(AST_RULES_FILENAME, "r");
	if (!astRulesFile)
	{
		printf("\nError: AST Rules file \"%s\" not found.\nExiting..........", AST_RULES_FILENAME);
		char c;
		scanf("%c", &c);
		return -1;
	}

	Parser parser;
	Lexer lex = lexer_createNew(pSourceFile, pKeywordsFile);


	printf("\n\nPlease enter your choice\n");
	printf("1. Print list of tokens.\n");
	printf("2. Verify syntactic correctness of input source code.\n");
	printf("3. Print Abstract Syntax Tree.\n");
	printf("4. Print Symbol Table.\n");
	printf("5. Verify syntactic and semantic correctness.\n");
	printf("6. Produce Assembly Code.\n");
	printf("7. Exit.\n");

	char flushChar;
	scanf("%d%c", &choice, &flushChar);
	switch (choice)
	{
	case 1:
		lexer_runLexicalAnalyses(lex);
		break;
	case 2:
		parser = parser_initialise(grammarInputFile, astRulesFile);

		isParsed = printOnlyParsetree(&parser, lex, DEFAULT_OUTPUT_PARSE_TREE_FILE, isParsed);
		break;
	case 3:
		parser = parser_initialise(grammarInputFile, astRulesFile);
		isParsed = parseOnlySourceCode(&parser, lex);
		if (isParsed == PARSED_SUCCESSFULLY)
		{
			Tree ast = createAst(parser);
			printAST(ast);
		}
		break;
	case 4:
		parser = parser_initialise(grammarInputFile, astRulesFile);
		isParsed = parseOnlySourceCode(&parser, lex);
		if (isParsed == PARSED_SUCCESSFULLY)
		{
			Tree ast = createAst(parser);
			createSymbolTables(ast);
			turnOnReportingSemanticErrors(FALSE);
			typeExtractorandChecker(ast);
			printSymbolTable();
		}
		break;
	case 5:
		parser = parser_initialise(grammarInputFile, astRulesFile);
		isParsed = parseOnlySourceCode(&parser, lex);
		if (isParsed == PARSED_SUCCESSFULLY)
		{
			Tree ast = createAst(parser);
			createSymbolTables(ast);
			printf("\n\nRunning semantic analysis....");
			turnOnReportingSemanticErrors(TRUE);
			if (typeExtractorandChecker(ast))
			{
				printf("\n\nCode compiles successfully..........:)");
			}
		}
		break;
	case 6:
		parser = parser_initialise(grammarInputFile, astRulesFile);
		isParsed = parseOnlySourceCode(&parser, lex);
		if (isParsed == PARSED_SUCCESSFULLY)
		{
			Tree ast = createAst(parser);
			createSymbolTables(ast);
			turnOnReportingSemanticErrors(TRUE);
			if (typeExtractorandChecker(ast))
			{
				printf("\n\nCode compiles successfully..........:)");
				printf("\n\nGenerating Code......");
				FILE* asmm = fopen(argv[2], "w");
				generateCode(ast, asmm);
				printf("\n\nFinished code generation.");
				//Call code generation here
			}
		}
		break;
	case 7:
		return 0;
		break;
	default:
		printf("Invalid Choice...please enter correct choice (1-7)");
		break;
	}

	char c;
	scanf("%c", &c);
	return 0;
}
