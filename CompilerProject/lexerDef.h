/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/

#ifndef _LEXERDEF_
#define _LEXERDEF_

#include "definitions.h"

#define BUFFER_SIZE 10 * 1024
#define MAX_LINE_SIZE 10 * 1024

typedef int Buffersize;
typedef char* Buffer;



typedef struct  
{
	DFA* pDFA; //Pointer to DFA which is used by this lexical analyzer

	//Twin buffer scheme has been used. This is the upper buffer and input reading starts from this buffer.
	//When this lexer has read from this buffer, it starts reading from the lower buffer, instead of waiting for I/O.
	//As the lexer starts reading from the lower buffer, command is issued to read more file input to upper buffer.
	Buffer upperBuffer;
						
	//Twin buffer scheme has been used.
	//When this lexer has read from the upper buffer, it starts reading from the lower buffer, instead of waiting for I/O into upper buffer.
	//As the lexer starts reading from the lower buffer, command is issued to read more file input to upper buffer.
	//When this lexer has read from the lower buffer also, it starts reading from the upper buffer.
	Buffer lowerBuffer;

	//The buffer which holds the input which has been read so far since tokenizing last lexeme.
	Buffer lexemeBuffer;

	//This field is a pointer to either one of upper buffer or lower buffer depending on which buffer is currently being used by the
	//lexer to read input.
	Buffer currentBuffer;

	//The source code file
	FILE* pSourceFile;

	//Indicates whether input has ended
	int moreTokensPossible;

	//Index corresponding to the current buffer from which input is being read.
	int mainBufferIndex;

	//Index corresponding to the lexeme buffer.
	int lexemeBufferIndex;

	//A lookup table which has TokenInfo entries. They store keywords as lexemes and also corresponding token.
	//The table is hashed by keywords (lexemes).
	HashTable keyWordTable;

	//The line no. of the source code which is currently being read.
	int lineNo;
} Lexer;

#endif