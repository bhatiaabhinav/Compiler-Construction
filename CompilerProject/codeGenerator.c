#include "codeGeneratorDef.h"
#include "codeGenerator.h"
#include "ASTDef.h"
#include "typeChecker.h"

void generateCode(Tree ast, FILE* asmm )
{
	//asmm = fopen("code.asmm", "w");
	int i;
	FILE* fin;
	initRegisters();
	labelNum = 0;
	fprintf(asmm, ".model tiny\n");
	fprintf(asmm, ".stack blah blah\n\n");
	fprintf(asmm, ".data\n");
	//dataPtr = *asmm;
	//declarationInfo(ast.root,asmm);
	for (i = 0; i < ast.root->no_of_children; i++)
	{
		declarationInfo(ast.root->children[i], asmm);
	}
	fprintf(asmm, "tempStr db 21 dup(?)\n");
	fprintf(asmm, "tempStr2 db 21 dup(?)\n");
	fprintf(asmm, "tempStr3 db 21 dup(?)\n");
	fprintf(asmm, "tempMatrix dw 100 dup(0)\n");
	fprintf(asmm, "tempMatrix2 dw 100 dup(0)\n");
	fprintf(asmm, "matrixAddress dw ? \n");
	fprintf(asmm, "matrixSize dw ? \n");
	fprintf(asmm, "strAddress dw ? \n");
	fprintf(asmm, "destStrAddress dw ? \n");
	fprintf(asmm, "\n.code\n");
	fprintf(asmm, "\n.startup\n");
	fin = fopen("iocode.txt", "r");
	
	
	
	
	
	for (i = 0; i < ast.root->no_of_children; i++)
	{
		_generateCode(ast.root->children[i],asmm);
	}
	fprintf(asmm,".exit\n");
	fprintf(asmm, "\n\n");
	initBasicIo(fin, asmm);

	//fprintf(asmm, ".end\n");
	fclose(fin);
	fclose(asmm);
}


void initRegisters()
{
	strcpy(reg[0], "AX");
	strcpy(reg[1], "BX");
	strcpy(reg[2], "CX");
	strcpy(reg[3], "DX");

	int i;
	for (i = 0; i < 4; i++)
	{
		isRegisterEmpty[i] = 1;
	}

	return;
}

int getEmptyRegister()
{
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		if (isRegisterEmpty[i]==1)
		{
			isRegisterEmpty[i] = 0;
			return i;
		}

	}

	return -1;
}

void declarationInfo(TreeLink astRoot, FILE* asmm)
{
	int i;
	ASTNodeData* currData;
	if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "<declarationStmt>") == 0)
	{
		//int i;
		currData = astRoot->children[0]->data;
		if (strcmp(currData->symbol->symbolName,"INT")==0)
		{
			for (i = 1; i < astRoot->no_of_children; i++)
			{
				//dataSegmentInfoAdd(astRoot->children[i]);
				//fprintf(asmm, ((ASTNodeData*)astRoot->data)->tokenInfo.lexeme);
				fprintf(asmm, "%s dw ? \n", ((ASTNodeData*)astRoot->children[i]->data)->tokenInfo.lexeme);

			}
		}
		else if (strcmp(currData->symbol->symbolName, "REAL") == 0)
		{
			for (i = 1; i < astRoot->no_of_children; i++)
			{
				//dataSegmentInfoAdd(astRoot->children[i]);
				//fprintf(asmm, ((ASTNodeData*)astRoot->data)->tokenInfo.lexeme);
				fprintf(asmm, "%s dw 2 dup(?) \n", ((ASTNodeData*)astRoot->children[i]->data)->tokenInfo.lexeme);

			}
		}

		else if (strcmp(currData->symbol->symbolName, "STRING") == 0)
		{
			for (i = 1; i < astRoot->no_of_children; i++)
			{
				//dataSegmentInfoAdd(astRoot->children[i]);
				//fprintf(asmm, ((ASTNodeData*)astRoot->data)->tokenInfo.lexeme);
				fprintf(asmm, "%s db 21 dup(?) \n", ((ASTNodeData*)astRoot->children[i]->data)->tokenInfo.lexeme);

			}
		}

		else if (strcmp(currData->symbol->symbolName, "MATRIX") == 0)
		{
			int sizeMatrix;
			SymbolTableEntry* newEntry;
			
			for (i = 1; i < astRoot->no_of_children; i++)
			{
				newEntry = findSymbolTableEntry(astRoot->children[i]->data);
				sizeMatrix = newEntry->numCols;
				sizeMatrix = sizeMatrix *newEntry->numRows;
				//dataSegmentInfoAdd(astRoot->children[i]);
				//fprintf(asmm, ((ASTNodeData*)astRoot->data)->tokenInfo.lexeme);
				fprintf(asmm, "%s dw %d dup(?) \n", ((ASTNodeData*)astRoot->children[i]->data)->tokenInfo.lexeme,sizeMatrix);

			}

		}

				
	}
	else
	{
		for (i = 0; i < astRoot->no_of_children; i++)
		{
			declarationInfo(astRoot->children[i],asmm);
		}
	}

}

void initBasicIo(FILE* inp,FILE* asmm)
{
	int c;
	c = fgetc(inp);
	while (c != EOF)
	{
		fputc(c, asmm);
		c = fgetc(inp);
	}
	//printf("lol");
}

void _generateCode(TreeLink astRoot,FILE* asmm)
{
	Symbol* rhsSymbol, *lhsSymbol;
	ASTNodeData *rhsData, *lhsData;
	int emptyRegister;
	char operation[10];
	int reg1, reg2;

	if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "<declarationStmt>") == 0)
	{
		return;
	}

	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "<ifBody>") == 0 || strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "<elseBody>") == 0)
	{
		int i;
		//i = astRoot->no_of_children;
		for (i = 0; i < astRoot->no_of_children; i++)
		{
			_generateCode(astRoot->children[i], asmm);
		}
		return;
	}

	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "ASSIGNOP") == 0)
	{
		if (((ASTNodeData*)astRoot->children[0]->data)->type == TYPE_INT)
		{
			_generateCode(astRoot->children[1], asmm);
			emptyRegister = getEmptyRegister();

			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			fprintf(asmm, "POP %s\n", reg[emptyRegister]);
			fprintf(asmm, "MOV %s, %s\n", ((ASTNodeData*)astRoot->children[0]->data)->tokenInfo.lexeme, reg[emptyRegister]);
			isRegisterEmpty[emptyRegister] = 1;
		}

		else if (((ASTNodeData*)astRoot->children[0]->data)->type == TYPE_REAL)
		{
			int reg3;
			_generateCode(astRoot->children[1], asmm);
			emptyRegister = getEmptyRegister();

			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg1 = emptyRegister;

			emptyRegister = getEmptyRegister();

			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg2 = emptyRegister;
			/*emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg3 = emptyRegister;*/

			fprintf(asmm, "POP %s\n", reg[reg2]);
			fprintf(asmm, "POP %s\n", reg[reg1]);
			//fprintf(asmm, "MOV %s,0\n", reg[reg3]);
			fprintf(asmm, "LEA si, %s\n", ((ASTNodeData*)astRoot->children[0]->data)->tokenInfo.lexeme);
			fprintf(asmm, "MOV [SI], %s\n", reg[reg1]);
			fprintf(asmm, "MOV [SI+2], %s\n", reg[reg2]);
			isRegisterEmpty[reg1] = 1;
			isRegisterEmpty[reg2] = 1;
			//isRegisterEmpty[reg3] = 1;
		}

		else if (((ASTNodeData*)astRoot->children[0]->data)->type == TYPE_STRING)
		{
			_generateCode(astRoot->children[1], asmm);
			//the left one is some string which wants to catch
			fprintf(asmm, "lea si,%s\n", ((ASTNodeData*)astRoot->children[0]->data)->tokenInfo.lexeme);
			fprintf(asmm, "mov strAddress,si\n");
			fprintf(asmm, "call copy_from_tempStr\n");
		}

		else if (((ASTNodeData*)astRoot->children[0]->data)->type == TYPE_MATRIX)
		{
			SymbolTableEntry* newEntry;
			_generateCode(astRoot->children[1], asmm);
			newEntry = findSymbolTableEntry(astRoot->children[0]->data);
			
			fprintf(asmm,"lea si,%s\n", ((ASTNodeData*)astRoot->children[0]->data)->tokenInfo.lexeme);
			fprintf(asmm,"mov matrixAddress,si\n");
			fprintf(asmm,"mov matrixSize,%d\n", newEntry->numCols*newEntry->numRows);
			fprintf(asmm,"call copyFromtempMatrix\n");
			
			
		}

		

	}

	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "<matrix>") == 0)
	{
		ASTNodeData* currData;
		TreeLink rowLink;
		int i,j;
		int tempLocation = 0;
		fprintf(asmm, "lea si,tempMatrix\n");
		for (i = 0; i < astRoot->no_of_children; i++)
		{
			rowLink = astRoot->children[i];
			//printf("%s\n", ((ASTNodeData*)astRoot->children[i]->data)->symbol->symbolName);
			for (j = 0; j < rowLink->no_of_children; j++)
			{
				//printf("%s\n", ((ASTNodeData*)rowLink->children[j]->data)->symbol->symbolName);
				currData = rowLink->children[j]->data;
				//printf("%s\n", (currData->tokenInfo.lexeme));
				fprintf(asmm, "mov [si+%d],%d\n", tempLocation, atoi(currData->tokenInfo.lexeme));
				tempLocation += 2;
			}
		}
	}

	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "GT") == 0 || strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "GE") == 0)
	{
		int currLabel;
		int reg3, reg4;
		currLabel = labelNum++;
		if (((ASTNodeData*)astRoot->children[0]->data)->type == TYPE_REAL)
		{
			_generateCode(astRoot->children[0], asmm);
			_generateCode(astRoot->children[1], asmm);

			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg1 = emptyRegister;

			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg2 = emptyRegister;

			/////////////////////////////////////////////////////////////////////////

			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg3 = emptyRegister;

			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg4 = emptyRegister;
			//decimal part of 2 child
			fprintf(asmm,"POP %s\n", reg[reg4]);
			//intergral part 
			fprintf(asmm,"POP %s\n", reg[reg3]);

			fprintf(asmm,"POP %s\n", reg[reg2]);
			fprintf(asmm,"POP %s\n", reg[reg1]);

			fprintf(asmm, "cmp %s,%s\n", reg[reg1], reg[reg3]);
			fprintf(asmm, "je fractCompGTE_%d\n", currLabel);
			fprintf(asmm, "JG finallyGreater_%d\n", currLabel);
			fprintf(asmm, "jmp finallyLess_%d\n", currLabel);
			
			fprintf(asmm, "fractCompGTE_%d:\n", currLabel);
			fprintf(asmm, "cmp %s,%s\n", reg[reg2], reg[reg4]);
			fprintf(asmm, "je finallyEqual_%d\n", currLabel);
			fprintf(asmm, "JG finallyGreater_%d\n", currLabel);

			fprintf(asmm, "finallyLess_%d:\n", currLabel);
			fprintf(asmm, "mov %s,0\n", reg[reg1]);
			fprintf(asmm, "PUSH %s\n", reg[reg1]);
			fprintf(asmm, "jmp endPartGT_%d\n", currLabel);

			fprintf(asmm, "finallyGreater_%d:\n", currLabel);
			fprintf(asmm, "mov %s,1\n", reg[reg1]);
			fprintf(asmm, "PUSH %s\n", reg[reg1]);
			fprintf(asmm, "jmp endPartGT_%d\n", currLabel);

			fprintf(asmm, "finallyEqual_%d:\n", currLabel);
			if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "GT") == 0)
			{
				fprintf(asmm, "mov %s,0\n", reg[reg1]);
			}
			else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "GE") == 0)
			{
				fprintf(asmm, "mov %s,1\n", reg[reg1]);
			}
			
			fprintf(asmm, "PUSH %s\n", reg[reg1]);
			fprintf(asmm, "jmp endPartGT_%d\n", currLabel);

			fprintf(asmm, "endPartGT_%d:\n\n", currLabel);

			isRegisterEmpty[reg1] = 1;
			isRegisterEmpty[reg2] = 1;
			isRegisterEmpty[reg3] = 1;
			isRegisterEmpty[reg4] = 1;



		}
		////////////////////////////////else part///////////////////////
		else
		{
			_generateCode(astRoot->children[0], asmm);
			_generateCode(astRoot->children[1], asmm);
			emptyRegister = getEmptyRegister();

			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg1 = emptyRegister;

			emptyRegister = getEmptyRegister();

			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg2 = emptyRegister;
			fprintf(asmm, "POP %s\n", reg[reg2]);
			fprintf(asmm, "POP %s\n", reg[reg1]);
			fprintf(asmm, "cmp %s,%s\n", reg[reg1], reg[reg2]);
			if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "GT") == 0)
			{
				fprintf(asmm, "JG greaterThanLabel_%d\n ", currLabel);
			}
			else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "GE") == 0)
			{
				fprintf(asmm, "JGE greaterThanLabel_%d\n ", currLabel);
			}

			fprintf(asmm, "mov %s,0\n", reg[reg1]);
			fprintf(asmm, "PUSH %s\n", reg[reg1]);
			fprintf(asmm, "jmp greaterThanEndPart_%d\n", currLabel);

			fprintf(asmm, "greaterThanLabel_%d:\n", currLabel);
			fprintf(asmm, "mov %s,1\n", reg[reg1]);
			fprintf(asmm, "PUSH %s\n", reg[reg1]);
			//fprintf(asmm, "jmp greaterThanendPart_%d", currLabel);
			fprintf(asmm, "greaterThanEndPart_%d:\n", currLabel);

			isRegisterEmpty[reg1] = 1;
			isRegisterEmpty[reg2] = 1;
		}
		
	}

	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "LT") == 0 || strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "LE") == 0)
	{
		int currLabel;
		currLabel = labelNum++;
		int reg3, reg4;

		if (((ASTNodeData*)astRoot->children[0]->data)->type == TYPE_REAL)
		{
			_generateCode(astRoot->children[0], asmm);
			_generateCode(astRoot->children[1], asmm);

			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg1 = emptyRegister;

			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg2 = emptyRegister;

			/////////////////////////////////////////////////////////////////////////

			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg3 = emptyRegister;

			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg4 = emptyRegister;
			//decimal part of 2 child
			fprintf(asmm, "POP %s\n", reg[reg4]);
			//intergral part 
			fprintf(asmm, "POP %s\n", reg[reg3]);

			fprintf(asmm, "POP %s\n", reg[reg2]);
			fprintf(asmm, "POP %s\n", reg[reg1]);

			fprintf(asmm, "cmp %s,%s\n", reg[reg1], reg[reg3]);
			fprintf(asmm, "je fractCompLTE_%d\n", currLabel);
			fprintf(asmm, "JG finallyGreater_%d\n", currLabel);
			fprintf(asmm, "jmp finallyLess_%d\n", currLabel);

			fprintf(asmm, "fractCompLTE_%d:\n", currLabel);
			fprintf(asmm, "cmp %s,%s\n", reg[reg2], reg[reg4]);
			fprintf(asmm, "je finallyEqual_%d\n", currLabel);
			fprintf(asmm, "JG finallyGreater_%d\n", currLabel);

			fprintf(asmm, "finallyLess_%d:\n", currLabel);
			fprintf(asmm, "mov %s,1\n", reg[reg1]);
			fprintf(asmm, "PUSH %s\n", reg[reg1]);
			fprintf(asmm, "jmp endPartLT_%d\n", currLabel);

			fprintf(asmm, "finallyGreater_%d:\n", currLabel);
			fprintf(asmm, "mov %s,0\n", reg[reg1]);
			fprintf(asmm, "PUSH %s\n", reg[reg1]);
			fprintf(asmm, "jmp endPartLT_%d\n", currLabel);

			fprintf(asmm, "finallyEqual_%d:\n", currLabel);
			if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "LT") == 0)
			{
				fprintf(asmm, "mov %s,0\n", reg[reg1]);
			}
			else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "LE") == 0)
			{
				fprintf(asmm, "mov %s,1\n", reg[reg1]);
			}

			fprintf(asmm, "PUSH %s\n", reg[reg1]);
			fprintf(asmm, "jmp endPartLT_%d\n", currLabel);

			fprintf(asmm, "endPartLT_%d:\n\n", currLabel);

			isRegisterEmpty[reg1] = 1;
			isRegisterEmpty[reg2] = 1;
			isRegisterEmpty[reg3] = 1;
			isRegisterEmpty[reg4] = 1;



		}
		////////////////////////////////else part/////////////////////////////////////////////
		else
		{
			_generateCode(astRoot->children[0], asmm);
			_generateCode(astRoot->children[1], asmm);
			emptyRegister = getEmptyRegister();

			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg1 = emptyRegister;

			emptyRegister = getEmptyRegister();

			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg2 = emptyRegister;
			fprintf(asmm, "POP %s\n", reg[reg2]);
			fprintf(asmm, "POP %s\n", reg[reg1]);
			fprintf(asmm, "cmp %s,%s\n", reg[reg1], reg[reg2]);
			if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "LT") == 0)
			{
				fprintf(asmm, "JL lessThanLabel_%d\n ", currLabel);
			}
			else
			{
				fprintf(asmm, "JLE lessThanLabel_%d\n ", currLabel);
			}


			fprintf(asmm, "mov %s,0\n", reg[reg1]);
			fprintf(asmm, "PUSH %s\n", reg[reg1]);
			fprintf(asmm, "jmp lessThanEndPart_%d\n", currLabel);

			fprintf(asmm, "lessThanLabel_%d:\n", currLabel);
			fprintf(asmm, "mov %s,1\n", reg[reg1]);
			fprintf(asmm, "PUSH %s\n", reg[reg1]);
			//fprintf(asmm, "jmp greaterThanendPart_%d", currLabel);
			fprintf(asmm, "lessThanEndPart_%d:\n\n", currLabel);

			isRegisterEmpty[reg1] = 1;
			isRegisterEmpty[reg2] = 1;
		}
		
		
	}

	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "AND") == 0 || strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "OR") == 0)
	{
		_generateCode(astRoot->children[0], asmm);
		_generateCode(astRoot->children[1], asmm);
		emptyRegister = getEmptyRegister();

		if (emptyRegister == -1)
		{
			printf("How come no register empty??");
			getchar();
			return;
		}
		reg1 = emptyRegister;

		emptyRegister = getEmptyRegister();

		if (emptyRegister == -1)
		{
			printf("How come no register empty??");
			getchar();
			return;
		}
		reg2 = emptyRegister;

		fprintf(asmm, "POP %s\n", reg[reg2]);
		fprintf(asmm, "POP %s\n", reg[reg1]);
		if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "AND") == 0)
		{
			fprintf(asmm, "AND %s,%s\n", reg[reg1], reg[reg2]);
		}
		else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "OR") == 0)
		{
			fprintf(asmm, "OR %s,%s\n", reg[reg1], reg[reg2]);
		}
		
		fprintf(asmm, "PUSH %s\n", reg[reg1]);

		isRegisterEmpty[reg1] = 1;
		isRegisterEmpty[reg2] = 1;
	}

	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "NOT") == 0)
	{
		int currLabel = labelNum++;
		_generateCode(astRoot->children[0], asmm);
		if (emptyRegister == -1)
		{
			printf("How come no register empty??");
			getchar();
			return;
		}
		reg1 = emptyRegister;
		fprintf(asmm, "POP %s\n", reg[reg1]);
		fprintf(asmm, "cmp %s,0\n", reg[reg1]);
		fprintf(asmm, "je notLabel_%d\n", currLabel);
		
		fprintf(asmm, "mov %s,0\n", reg[reg1]);
		fprintf(asmm, "PUSH %s\n", reg[reg1]);
		fprintf(asmm, "jmp notLabelEnd_%d", currLabel);
		
		fprintf(asmm, "notLabel_%d:\n", currLabel);
		fprintf(asmm, "mov %s,1\n", reg[reg1]);
		fprintf(asmm, "PUSH %s\n", reg[reg1]);
		
		fprintf(asmm, "notLabelEnd_%d:\n\n");


		isRegisterEmpty[reg1] = 1;

	}

	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "EQ") == 0 || strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "NE") == 0)
	{
		int currLabel = labelNum++;
		int reg3, reg4;

		if (((ASTNodeData*)astRoot->children[0]->data)->type == TYPE_REAL)
		{
			_generateCode(astRoot->children[0], asmm);
			_generateCode(astRoot->children[1], asmm);

			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg1 = emptyRegister;

			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg2 = emptyRegister;

			/////////////////////////////////////////////////////////////////////////

			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg3 = emptyRegister;

			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg4 = emptyRegister;
			//decimal part of 2 child
			fprintf(asmm, "POP %s\n", reg[reg4]);
			//intergral part 
			fprintf(asmm, "POP %s\n", reg[reg3]);

			fprintf(asmm, "POP %s\n", reg[reg2]);
			fprintf(asmm, "POP %s\n", reg[reg1]);

			fprintf(asmm, "cmp %s,%s\n", reg[reg1], reg[reg3]);
			fprintf(asmm, "je nextCheck_%d\n", currLabel);
			fprintf(asmm, "jmp notEqual_%d\n", currLabel);
			
			fprintf(asmm, "nextCheck_%d:\n", currLabel);
			fprintf(asmm, "cmp %s,%s\n", reg[reg1], reg[reg3]);
			fprintf(asmm, "je finallyEqual_%d\n", currLabel);

			fprintf(asmm, "notEqual_%d:\n", currLabel);
			if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "EQ") == 0)
			{
				fprintf(asmm, "mov %s,0\n", reg[reg1]);
				fprintf(asmm, "PUSH %s\n", reg[reg1]);
			}
			else
			{
				fprintf(asmm, "mov %s,1\n", reg[reg1]);
				fprintf(asmm, "PUSH %s\n", reg[reg1]);
			}
			fprintf(asmm, "jmp endPart_%d\n", currLabel);

			fprintf(asmm, "finallyEqual_%d:\n", currLabel);
			if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "EQ") == 0)
			{
				fprintf(asmm, "mov %s,1\n", reg[reg1]);
				fprintf(asmm, "PUSH %s\n", reg[reg1]);
			}
			else
			{
				fprintf(asmm, "mov %s,0\n", reg[reg1]);
				fprintf(asmm, "PUSH %s\n", reg[reg1]);
			}

			fprintf(asmm, "endPart_%d:\n\n", currLabel);




			isRegisterEmpty[reg1] = 1;
			isRegisterEmpty[reg2] = 1;
			isRegisterEmpty[reg3] = 1;
			isRegisterEmpty[reg4] = 1;

		}

		/////////////////////////////elsepart//////////////////////////////
		
		else
		{
			_generateCode(astRoot->children[0], asmm);
			_generateCode(astRoot->children[1], asmm);
			emptyRegister = getEmptyRegister();

			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg1 = emptyRegister;

			emptyRegister = getEmptyRegister();

			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg2 = emptyRegister;

			fprintf(asmm, "POP %s\n", reg[reg2]);
			fprintf(asmm, "POP %s\n", reg[reg1]);
			fprintf(asmm, "cmp %s,%s", reg[reg1], reg[reg2]);
			if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "EQ") == 0)
			{
				fprintf(asmm, "je equalLabel_%d", currLabel);
				fprintf(asmm, "mov %s,0\n", reg[reg1]);
				fprintf(asmm, "PUSH %s\n", reg[reg1]);
				fprintf(asmm, "jmp equalEndLabel_%d\n", currLabel);

				fprintf(asmm, "equalLabel_%d:\n", currLabel);
				fprintf(asmm, "mov %s,1\n", reg[reg1]);
				fprintf(asmm, "PUSH %s\n", reg[reg1]);

				fprintf(asmm, "equalEndLabel_%d:\n\n", currLabel);

			}

			if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "NE") == 0)
			{
				fprintf(asmm, "jne notEqualLabel_%d", currLabel);
				fprintf(asmm, "mov %s,0\n", reg[reg1]);
				fprintf(asmm, "PUSH %s\n", reg[reg1]);
				fprintf(asmm, "jmp notEqualEndLabel_%d\n", currLabel);

				fprintf(asmm, "notEqualLabel_%d:\n", currLabel);
				fprintf(asmm, "mov %s,1\n", reg[reg1]);
				fprintf(asmm, "PUSH %s\n", reg[reg1]);

				fprintf(asmm, "notEqualEndLabel_%d:\n\n", currLabel);

			}
		}
		
	}


	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "<ifStmt>") == 0)
	{
		int currLabelNum;
		currLabelNum = labelNum++;
		_generateCode(astRoot->children[0], asmm);
		emptyRegister = getEmptyRegister();

		if (emptyRegister == -1)
		{
			printf("How come no register empty??");
			getchar();
			return;
		}
		reg1 = emptyRegister;
		fprintf(asmm, "POP %s\n", reg[reg1]);
		fprintf(asmm, "cmp %s,0\n", reg[reg1]);
		fprintf(asmm, "je elsePart_%d\n", currLabelNum);
		isRegisterEmpty[reg1] = 1;
		_generateCode(astRoot->children[1],asmm);
		fprintf(asmm, "jmp endIf_%d\n", currLabelNum);
		fprintf(asmm, "elsePart_%d:\n", currLabelNum);
		if (astRoot->no_of_children == 3)
		{
			_generateCode(astRoot->children[2], asmm);
		}
		fprintf(asmm, "endIf_%d:\n\n", currLabelNum);
	}



	//fi you are alone and you are explicit  num
	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "NUM") == 0)
	{
		emptyRegister = getEmptyRegister();

		if (emptyRegister == -1)
		{
			printf("How come no register empty??");
			getchar();
			return;
		}
		fprintf(asmm, "MOV %s, %s\n", reg[emptyRegister], ((ASTNodeData*)astRoot->data)->tokenInfo.lexeme);
		fprintf(asmm, "PUSH %s\n", reg[emptyRegister]);
		isRegisterEmpty[emptyRegister] = 1;
		return;
	}

	//if you are alone and you are explicit rnum
	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "RNUM") == 0 )
	{
		//first integral part is pushed then decimal part
		char integerPart[10], decimalPart[10], symbolName[20];
		strcpy(symbolName, ((ASTNodeData*)astRoot->data)->tokenInfo.lexeme);
		int i,j;
		emptyRegister = getEmptyRegister();
		//fprintf(asmm, "in single rnum\n");
		if (emptyRegister == -1)
		{
			printf("How come no register empty??");
			getchar();
			return;
		}
		reg1 = emptyRegister;

		emptyRegister = getEmptyRegister();

		if (emptyRegister == -1)
		{
			printf("How come no register empty??");
			getchar();
			return;
		}
		reg2 = emptyRegister;

		for (i = 0; symbolName[i] != '.'; i++)
		{
			integerPart[i] = symbolName[i];
		}
		integerPart[i] = '\0';
		i++;
		for (j = 0; j < 2; j++)
		{
			decimalPart[j] = symbolName[i++];
		}
		decimalPart[j] = '\0';

		fprintf(asmm, "MOV %s, %s\n", reg[reg1], integerPart);
		fprintf(asmm, "MOV %s,%s\n", reg[reg2], decimalPart);
		fprintf(asmm, "PUSH %s\n", reg[reg1]);
		fprintf(asmm, "PUSH %s\n", reg[reg2]);
		isRegisterEmpty[reg1] = 1;
		isRegisterEmpty[reg2] = 1;
		return;

	}

	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "STR") == 0)
	{
		int i;
		char string[21];
		strcpy(string ,((ASTNodeData*)astRoot->data)->tokenInfo.lexeme);
		for (i = 0; i < strlen(string) - 1; i++)
		{
			string[i] = string[i + 1];
		}
		
		i = 0;
		fprintf(asmm,"; have to assign string\n");
		fprintf(asmm, "lea si,tempStr\n");
		while (string[i] != '"')
		{
			fprintf(asmm,"mov [si+%d],'%c'\n", i,string[i]);
			i++;
		}
		fprintf(asmm,"mov [si+%d],'$'\n",i);
	}
	
	//if you are alone but you are ID. you type is hidden a bit
	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "ID") == 0)
	{
		//type is int
		if (((ASTNodeData*)astRoot->data)->type == TYPE_INT)
		{
			emptyRegister = getEmptyRegister();

			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			fprintf(asmm, "MOV %s, %s\n", reg[emptyRegister], ((ASTNodeData*)astRoot->data)->tokenInfo.lexeme);
			fprintf(asmm, "PUSH %s\n", reg[emptyRegister]);
			isRegisterEmpty[emptyRegister] = 1;
		}

		else if (((ASTNodeData*)astRoot->data)->type == TYPE_MATRIX)
		{
			SymbolTableEntry* newEntry;
			newEntry = findSymbolTableEntry(((ASTNodeData*)astRoot->data));
			fprintf(asmm,"lea si,%s\n", ((ASTNodeData*)astRoot->data)->tokenInfo.lexeme);
			fprintf(asmm,"mov matrixAddress,si\n");
			fprintf(asmm,"mov matrixSize,%d\n", newEntry->numCols*newEntry->numRows);
			fprintf(asmm,"call copyTotempMatrix\n");
		}
		
		//type is real
		else if (((ASTNodeData*)astRoot->data)->type == TYPE_REAL)
		{
		
			fprintf(asmm, "lea SI,%s\n", ((ASTNodeData*)astRoot->data)->tokenInfo.lexeme);
			fprintf(asmm, "PUSH [SI]\n");
			fprintf(asmm, "PUSH [SI+2]\n");
			return;
		}

		else if (((ASTNodeData*)astRoot->data)->type == TYPE_STRING)
		{
			fprintf(asmm, "lea si,%s\n", ((ASTNodeData*)astRoot->data)->tokenInfo.lexeme);
			fprintf(asmm, "mov strAddress,si\n");
			fprintf(asmm, "lea di, tempStr\n");
			fprintf(asmm, "mov destStrAddress,di\n");
			fprintf(asmm, "call copyString\n");
		}
		
	}


	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "PLUS") == 0 || strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "MINUS") == 0)
	{ //if operator is + or -
		int reg3, reg4;
		if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "PLUS") == 0)
		{
			strcpy(operation, "ADD");
		}
		else
			strcpy(operation, "SUB");
		
		rhsSymbol = ((ASTNodeData*)astRoot->children[1]->data)->symbol;
		lhsSymbol = ((ASTNodeData*)astRoot->children[0]->data)->symbol;
		rhsData = astRoot->children[1]->data;
		lhsData = astRoot->children[0]->data;
		//if (strcmp(rhsSymbol->symbolName, "ID") == 0 || strcmp(rhsSymbol->symbolName, "NUM") == 0 )
		//{//if the RHS is ID or num or RNUM
		//	emptyRegister = getEmptyRegister();
		//	if (emptyRegister == -1)
		//	{
		//		printf("How come no register empty??");
		//		getchar();
		//		return;
		//	}
		//	reg1 = emptyRegister;
		//	fprintf(asmm, "MOV %s %s\n", reg[reg1], ((ASTNodeData*)astRoot->children[1]->data)->tokenInfo.lexeme);
		//	if (strcmp(((ASTNodeData*)astRoot->children[0]->data)->symbol->symbolName, "NUM") == 0)
		//	{
		//		emptyRegister = getEmptyRegister();
		//		if (emptyRegister == -1)
		//		{
		//			printf("How come no register empty??");
		//			getchar();
		//			return;
		//		}

		//		reg2 = emptyRegister;
		//		fprintf(asmm, "MOV %s %s", reg[reg2], ((ASTNodeData*)astRoot->children[0]->data)->tokenInfo.lexeme);
		//		fprintf(asmm, "%s %s %s \n", operation, reg[reg2], reg[reg1]);
		//		fprintf(asmm, "PUSH %s\n", reg[emptyRegister]);
		//		isRegisterEmpty[reg1] = 1;
		//		isRegisterEmpty[reg2] = 1;
		//	}
		//	else
		//	{
		//		fprintf(asmm, "%s %s %s \n", operation, ((ASTNodeData*)astRoot->children[0]->data)->tokenInfo.lexeme, reg[reg1]);
		//		fprintf(asmm, "PUSH %s\n", reg[reg1]);
		//		isRegisterEmpty[reg1] = 1;
		//		//isRegisterEmpty[emptyRegister] = 1;
		//	}

		//	
		//	
		//}
		/*********************************************************/
		if (rhsData->type == TYPE_MATRIX)
		{
			_generateCode(astRoot->children[1],asmm);
			fprintf(asmm,"call copyMatTemp1Temp2\n");
			_generateCode(astRoot->children[0], asmm);
			if (strcmp(operation, "ADD") == 0)
			{
				fprintf(asmm,"call addTemp1Temp2\n");
			}
			else
			{
				fprintf(asmm,"call subTemp1Temp2\n");
			}
			


		}

		else if (rhsData->type == TYPE_INT)
		{
			_generateCode(astRoot->children[0],asmm);
			_generateCode(astRoot->children[1], asmm);
			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg1 = emptyRegister;
			fprintf(asmm, "POP %s\n", reg[reg1]);
			
			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg2 = emptyRegister;
			fprintf(asmm, "POP %s\n", reg[reg2]);
			fprintf(asmm, "%s %s, %s\n", operation, reg[reg1], reg[reg2]);
			fprintf(asmm, "PUSH %s\n", reg[reg1]);
			isRegisterEmpty[reg1] = 1;
			isRegisterEmpty[reg2] = 1;

			
		}

		else if (rhsData->type == TYPE_STRING)
		{
			
			// tell the right one to do its work, its answer would be in tempStr
			_generateCode(astRoot->children[1], asmm);
			//move that string from tempStr to tempStr2
			fprintf(asmm,"lea si,tempStr\n");
			fprintf(asmm, "mov strAddress,si\n");
			fprintf(asmm, "lea di,tempStr3\n");
			fprintf(asmm, "mov destStrAddress,di\n");
			fprintf(asmm, "call copyString\n");

			_generateCode(astRoot->children[0], asmm);
			fprintf(asmm, "lea si,tempStr\n");
			fprintf(asmm, "mov strAddress,si\n");
			fprintf(asmm, "lea di,tempStr2\n");
			fprintf(asmm, "mov destStrAddress,di\n");
			fprintf(asmm, "call copyString\n");

			fprintf(asmm, "call strConcat\n");

		}

		else if (rhsData->type == TYPE_REAL)
		{
			int currLabel = labelNum++;
			_generateCode(astRoot->children[0], asmm);
			_generateCode(astRoot->children[1], asmm);


			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg1 = emptyRegister;

			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg2 = emptyRegister;

			/////////////////////////////////////////////////////////////////////////

			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg3 = emptyRegister;
			
			emptyRegister = getEmptyRegister();
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}
			reg4 = emptyRegister;
			
			fprintf(asmm, "POP %s\n", reg[reg4]);
			fprintf(asmm, "POP %s\n", reg[reg3]);
			fprintf(asmm, "POP %s\n", reg[reg2]);
			fprintf(asmm, "POP %s\n", reg[reg1]);
			/*fprintf(asmm, "%s %s,%s\n", operation, reg[reg2], reg[reg4]);
			if (strcmp(operation, "ADD") == 0)
			{
				fprintf(asmm, "ADC %s,%s\n", reg[reg1], reg[reg3]);
				
			}
			else
			{
				fprintf(asmm, "SBB %s,%s\n", reg[reg1], reg[reg3]);

			}*/
			if (strcmp(operation, "ADD") == 0)
			{
				fprintf(asmm, "ADD %s,%s\n", reg[reg2], reg[reg4]);
				fprintf(asmm, "cmp %s,100\n", reg[reg2]);
				fprintf(asmm, "jl nextAddPartNoCarry_%d\n", currLabel);
				fprintf(asmm, "SUB %s,100\n", reg[reg2]);
				//fprintf(asmm, "jmp addPartCarry_%d\n", currLabel);
				fprintf(asmm, "ADD %s,%s\n", reg[reg1], reg[reg3]);
				fprintf(asmm, "ADD %s,1\n", reg[reg1]);
				fprintf(asmm, "jmp endPart_%d\n", currLabel);
				fprintf(asmm, "nextAddPartNoCarry_%d:\n",currLabel);
				fprintf(asmm, "ADD %s,%s\n", reg[reg1], reg[reg3]);
				fprintf(asmm, "endPart_%d:\n",currLabel);
				fprintf(asmm, "PUSH %s\n", reg[reg1]);
				fprintf(asmm, "PUSH %s\n", reg[reg2]);
			}
			if (strcmp(operation, "SUB") == 0)
			{
				fprintf(asmm, "cmp %s,%s\n", reg[reg2], reg[reg4]);
				fprintf(asmm, "jl borrowWaalaCase_%d\n", currLabel);
				fprintf(asmm, "%s %s,%s\n", operation, reg[reg2], reg[reg4]);
				fprintf(asmm, "SUB %s,%s\n", reg[reg1], reg[reg3]);
				fprintf(asmm, "jmp endPart_%d\n", currLabel);
				fprintf(asmm, "borrowWaalaCase_%d:\n", currLabel);
				fprintf(asmm, "ADD %s,100\n", reg[reg2]);
				fprintf(asmm, "SUB %s,%s\n", reg[reg2], reg[reg4]);
				fprintf(asmm, "SUB %s,1\n", reg[reg1]);
				fprintf(asmm, "SUB %s,%s\n", reg[reg1], reg[reg3]);
				fprintf(asmm, "endPart_%d:\n", currLabel);
				fprintf(asmm, "PUSH %s\n", reg[reg1]);
				fprintf(asmm, "PUSH %s\n", reg[reg2]);


				/*fprintf(asmm, "%s %s,%s\n", operation, reg[reg2], reg[reg4]);
				fprintf(asmm, "SBB %s,%s\n", reg[reg1], reg[reg3]);
				fprintf(asmm, "PUSH %s\n", reg[reg1]);
				fprintf(asmm, "PUSH %s\n", reg[reg2]);*/
			}

			isRegisterEmpty[reg1] = 1;
			isRegisterEmpty[reg2] = 1;
			isRegisterEmpty[reg3] = 1;
			isRegisterEmpty[reg4] = 1;

		}

		/*********************************************************/
		//if ((rhsSymbol->symbolName=="ID"&&rhsData->type==TYPE_INT)||rhsSymbol->symbolName=="NUM")
		//{
		//	
		//	//the rhs is register in any case
		//	emptyRegister = getEmptyRegister();
		//	if (emptyRegister == -1)
		//	{
		//		printf("How come no register empty??");
		//		getchar();
		//		return;
		//	}
		//	reg1 = emptyRegister;
		//	fprintf(asmm, "MOV %s %s\n", reg[reg1], ((ASTNodeData*)astRoot->children[1]->data)->tokenInfo.lexeme);
		//	
		//	//if (strcmp(lhsSymbol->symbolName, "ID") == 0)
		//	//{
		//	//	// if lhs is ID
		//	//	fprintf(asmm, "%s %s %s \n", operation, ((ASTNodeData*)astRoot->children[0]->data)->tokenInfo.lexeme, reg[reg1]);
		//	//	fprintf(asmm, "PUSH %s\n", reg[reg1]);
		//	//	isRegisterEmpty[reg1] = 1;
		//	//}
		//	//else
		//	//{
		//		//lhs has to be num
		//		emptyRegister = getEmptyRegister();
		//		if (emptyRegister == -1)
		//		{
		//			printf("How come no register empty??");
		//			getchar();
		//			return;
		//		}

		//		reg2 = emptyRegister;
		//		fprintf(asmm, "MOV %s %s\n", reg[reg2], ((ASTNodeData*)astRoot->children[0]->data)->tokenInfo.lexeme);
		//		fprintf(asmm, "%s %s %s \n", operation, reg[reg2], reg[reg1]);
		//		fprintf(asmm, "PUSH %s\n", reg[emptyRegister]);
		//		isRegisterEmpty[reg1] = 1;
		//		isRegisterEmpty[reg2] = 1;
		//	//}
		//}
		//else if (strcmp(rhsSymbol->symbolName, "RNUM") == 0)
		//{
		//	
		//}
		//else if (strcmp(rhsSymbol->symbolName,"MATRIX")==0)
		//{

		//}
		//else if (strcmp(rhsSymbol->symbolName, "STR") == 0)
		//{

		//}
		//else
		//{
		//	//RHS is not directly ID, NUM or RNUM but something else

		//	//for some other operator as RHS 
		//	_generateCode(astRoot->children[1], asmm);
		//	emptyRegister = getEmptyRegister();
		//	if (emptyRegister == -1)
		//	{
		//		printf("How come no register empty??");
		//		getchar();
		//		return;
		//	}
		//	reg1 = emptyRegister;
		//	fprintf(asmm,"POP %s\n", reg[reg1]);
		//	//if (strcmp(((ASTNodeData*)astRoot->children[0]->data)->symbol->symbolName, "NUM") == 0)
		//	//{
		//		emptyRegister = getEmptyRegister();
		//		if (emptyRegister == -1)
		//		{
		//			printf("How come no register empty??");
		//			getchar();
		//			return;
		//		}

		//		reg2 = emptyRegister;
		//		fprintf(asmm, "MOV %s %s\n", reg[reg2], ((ASTNodeData*)astRoot->children[0]->data)->tokenInfo.lexeme);
		//		fprintf(asmm, "%s %s %s \n", operation, reg[reg2], reg[reg1]);
		//		fprintf(asmm, "PUSH %s\n", reg[emptyRegister]);
		//		isRegisterEmpty[reg1] = 1;
		//		isRegisterEmpty[reg2] = 1;
		//	//}
		//	/*else
		//	{
		//		fprintf(asmm, "%s %s %s \n", operation, ((ASTNodeData*)astRoot->children[0]->data)->tokenInfo.lexeme, reg[reg1]);
		//		fprintf(asmm, "PUSH %s\n", reg[reg1]);
		//		isRegisterEmpty[reg1] = 1;
		//	}*/
		//	
		//	

		//}

		

		return;
	}

	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "SIZE") == 0)
	{
		int sizeString;
		if (((ASTNodeData*)astRoot->children[0]->data)->type == TYPE_STRING)
		{
			SymbolTableEntry* newEntry = findSymbolTableEntry(astRoot->children[0]->data);
			//sizeString=((ASTNodeData*)astRoot->children[0]->data)->stringSize;
			sizeString = newEntry->stringSize;
			emptyRegister = getEmptyRegister();
			
			if (emptyRegister == -1)
			{
				printf("How come no register empty??");
				getchar();
				return;
			}

			reg1 = emptyRegister;
			fprintf(asmm, "mov %s,%d\n", reg[reg1], sizeString);
			fprintf(asmm, "push %s\n", reg[reg1]);
			isRegisterEmpty[reg1] = 1;
		}
	}
	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "MUL") == 0)
	{
		//didn't check multiplication overflow
		lhsData = astRoot->children[0]->data;
		rhsData = astRoot->children[0]->data;
		_generateCode(astRoot->children[0], asmm);
		
		_generateCode(astRoot->children[1], asmm);
		
		isRegisterEmpty[0] = 0;
		emptyRegister = getEmptyRegister();

		if (emptyRegister == -1)
		{
			printf("How come no register empty??");
			getchar();
			return;
		}

		reg2 = emptyRegister;

		fprintf(asmm, "POP %s\n", reg[reg2]);
		fprintf(asmm, "POP %s\n", "AX");
		fprintf(asmm, "MUL %s\n", reg[reg2]);
		fprintf(asmm, "PUSH %s\n", "AX");
		isRegisterEmpty[0] = 1;
		isRegisterEmpty[reg2] = 1;

		return;


	}

	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "DIV") == 0)
	{
		_generateCode(astRoot->children[0], asmm);
		
		_generateCode(astRoot->children[1], asmm);
		
		emptyRegister = getEmptyRegister();
		
		isRegisterEmpty[0] = 0;
		if (emptyRegister == -1)
		{
			printf("How come no register empty??");
			getchar();
			return;
		}

		reg2 = emptyRegister;

		fprintf(asmm, "POP %s\n", reg[reg2]);
		fprintf(asmm, "POP %s\n", "AX");
		fprintf(asmm, "DIV %s\n", reg[reg2]);
		fprintf(asmm, "PUSH %s\n", "AX");
		isRegisterEmpty[0] = 1;
		isRegisterEmpty[reg2] = 1;

		return;

	}

	else if (strcmp(((ASTNodeData*)astRoot->data)->symbol->symbolName, "PRINT") == 0)
	{
		
		if (((ASTNodeData*)astRoot->children[0]->data)->type == TYPE_INT)
		{
			_generateCode(astRoot->children[0], asmm);
			fprintf(asmm,"POP AX\n");
			fprintf(asmm,"call print_reg_asm\n");
			fprintf(asmm, "call print_newLine\n");
			//fprintf(asmm, "21h\n");
		}
		if (((ASTNodeData*)astRoot->children[0]->data)->type == TYPE_REAL)
		{
			_generateCode(astRoot->children[0], asmm);
			fprintf(asmm, "POP BX\n");
			fprintf(asmm, "POP AX\n");
			fprintf(asmm, "call print_reg_asm\n");
			fprintf(asmm, "MOV dl,'.'\n");
			fprintf(asmm, "mov ah,2h\n");
			fprintf(asmm, "int 21h\n");
			fprintf(asmm, "MOV AX,BX\n");
			fprintf(asmm, "call print_reg_asm\n");
			fprintf(asmm, "call print_newLine\n");


		}
		if ((((ASTNodeData*)astRoot->children[0]->data)->type == TYPE_STRING))
		{
			fprintf(asmm, "lea si,%s\n", ((ASTNodeData*)astRoot->children[0]->data)->tokenInfo.lexeme);
			fprintf(asmm, "mov strAddress,si\n");
			fprintf(asmm, "call printString\n");
		}

		if ((((ASTNodeData*)astRoot->children[0]->data)->type == TYPE_MATRIX))
		{
			_generateCode(astRoot->children[0], asmm);
			SymbolTableEntry* newEntry;
			int i, j;
			//fprintf(asmm, "lea si,%s\n", ((ASTNodeData*)astRoot->children[0]->data)->tokenInfo.lexeme);
			fprintf(asmm, "lea si,tempMatrix\n");
			newEntry = findSymbolTableEntry(((ASTNodeData*)astRoot->children[0]->data));
			for (i = 0; i < newEntry->numRows; i++)
			{
				for (j = 0; j < newEntry->numCols; j++)
				{
					fprintf(asmm, "mov bx,[si+%d]\n", 2*((i* newEntry->numCols ) + j));
					fprintf(asmm, "mov ax,bx\n");
					fprintf(asmm, "call print_reg_asm\n");
					fprintf(asmm, "MOV dl,' '\n");
					fprintf(asmm, "mov ah,2h\n");
					fprintf(asmm, "int 21h\n");

				}
				fprintf(asmm, "call print_newLine\n");
			}
		}
	}
	
}