CC = gcc
CFLAGS = -w -g
DEPS = compiler.o DFA.o  follow.o HashTable.o first.o lexer.o linklist.o parser.o set.o Tree.o AST.o codeGenerator.o SymbolTable.o typeChecker.o driver.o 

stage2exe:$(DEPS)
	$(CC) $(CFLAGS) -o stage2exe $(DEPS)

compiler.o: compiler.h compiler.c
	$(CC) $(CFLAGS) -c compiler.c

DFA.o: DFA.c DFA.h 
	$(CC) $(CFLAGS) -c DFA.c

first.o:  first.h set.h parser.h first.c compiler.h
	$(CC) $(CFLAGS) -c first.c

follow.o: follow.c follow.h compiler.h 
	$(CC) $(CFLAGS) -c follow.c
	
HashTable.o: HashTable.h HashTable.c
	$(CC) $(CFLAGS) -c HashTable.c

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c
	
linklist.o: linklist.c linklist.h
	$(CC) $(CFLAGS) -c linklist.c
	
parser.o: parser.c parser.h compiler.h first.h follow.h
	$(CC) $(CFLAGS) -c parser.c
	
set.o: set.c set.h 
	$(CC) $(CFLAGS) -c set.c
	
Tree.o: Tree.h Tree.c 
	$(CC) $(CFLAGS) -c Tree.c
	
AST.o: AST.c AST.h 
	$(CC) $(CFLAGS) -c AST.c

codeGenerator.o: codeGeneratorDef.h codeGenerator.c codeGenerator.h
	$(CC) $(CFLAGS) -c codeGenerator.c

typeChecker.o: typeChecker.c
	$(CC) $(CFLAGS) -c typeChecker.c
	
driver.o: driver.c compiler.h first.h follow.h
	$(CC) $(CFLAGS) -c driver.c
	
clean:
	-rm -f *.o core
	

