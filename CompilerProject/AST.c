/*
*Batch Number 6
*Abhinav Bhatia (2011A7PS371P)
*Mukul Bhutani (2011A7PS343P)
*/
#include "parser.h"
#include "AST.h"
#include "definitions.h"
#include "compiler.h"

Tree ASTConstructionDfs(TreeLink node)
{
	int i,j;
	//LinkList parentsLinkList = linkList_createNew(compareRulesByReference);
	
	TreeLink pNode;
	Tree temp;
	ParseTreeNodeData* childNodeData;
	//pNode = (TreeLink)malloc(sizeof(TreeNode));
	//ast = tree_add(ast,)
	Tree subTree;
	subTree = tree_createNew();
	subTree = tree_add(subTree, NULL, NULL);
	pNode = subTree.root;
	pNode->data = (ASTNodeData*)malloc(sizeof(ASTNodeData));
	((ASTNodeData*)pNode->data)->symbol = ((ParseTreeNodeData*)node->data)->symbol;
	((ASTNodeData*)pNode->data)->tokenInfo = ((ParseTreeNodeData*)node->data)->tokenInfo;
	((ASTNodeData*)pNode->data)->type = TYPE_ERROR;
	((ASTNodeData*)pNode->data)->vectorType = linkList_createNew(typeComparator);
	((ASTNodeData*)pNode->data)->numOfRows = 0;
	((ASTNodeData*)pNode->data)->numOfCols = 0;
	((ASTNodeData*)pNode->data)->offset = 0;
	((ASTNodeData*)pNode->data)->stringSize = 0;

	for (i = 0; i < node->no_of_children; i++)
	{
		childNodeData = (ParseTreeNodeData*)node->children[i]->data;
		if (childNodeData->usefulNess == useless)
			continue;
		else if (childNodeData->usefulNess == concrete)
		{
			temp = ASTConstructionDfs(node->children[i]);
			{
				pNode->children[pNode->no_of_children++] = temp.root;
				subTree.totalNodes += temp.totalNodes;
				if (subTree.totalLeafNodes > 1)
					subTree.totalLeafNodes += temp.totalLeafNodes;
				else
					subTree.totalLeafNodes += temp.totalLeafNodes - 1;
			}
		}
		else if (childNodeData->usefulNess==forwarded)
		{
			temp = ASTConstructionDfs(node->children[i]);
			for (j = 0; j < temp.root->no_of_children; j++)
			{
				pNode->children[pNode->no_of_children++] = temp.root->children[j];
			}
			if (subTree.totalLeafNodes>1)
				subTree.totalLeafNodes += temp.totalLeafNodes;
			else
				subTree.totalLeafNodes += temp.totalLeafNodes - 1;

			subTree.totalNodes += temp.totalNodes;

		}
		else
		{
			printf("There was an error reading usefullness for symbol %s for rule no %d", childNodeData->symbol->symbolName, childNodeData->expandsFrom->ruleNo);
		}
	}

	return subTree;
}

Tree createAst(Parser parser)
{
	printf("\nCreating Abstract Syntax Tree...");
	Tree ast;

	ast = ASTConstructionDfs(parser.parseTree.root->children[0]);
	ast = clearAst(ast);
	printf("\nAbstract Syntax created.");
	return ast;
}

int toPush(char* string)
{
	if (strcmp(string, "PLUS") == 0 || strcmp(string, "MINUS") == 0 || strcmp(string, "DIV") == 0 || strcmp(string, "MUL") == 0 || strcmp(string, "ASSIGNOP") == 0
		|| strcmp(string, "PRINT") == 0 || strcmp(string, "READ") == 0 || strcmp(string, "AND") == 0 || strcmp(string, "OR") == 0 || strcmp(string, "LT") == 0
		|| strcmp(string, "LE") == 0 || strcmp(string, "EQ") == 0 || strcmp(string, "GT") == 0 || strcmp(string, "GE") == 0 || strcmp(string, "NE") == 0 || strcmp(string, "NOT") == 0
		|| strcmp(string, "SIZE") == 0)
		return 1;
	else return 0;
}

void removeChild(TreeLink root, int i)
{
	TreeLink temp;
	int j;
	if (i > root->no_of_children)
		return;
	temp = root->children[i];
	for (j = i; j < root->no_of_children - 1; j++)
	{
		root->children[j] = root->children[j + 1];
	}

	root->no_of_children--;
	free(temp);

}

int cannotBeDeleted(char* string)
{
	return strcmp(string, "<row>") == 0 || strcmp(string, "<matrix>") == 0 || strcmp(string, "<funCallStmt>") == 0
		|| strcmp(string, "<parameter_list>") == 0 || strcmp(string, "<ifBody>") == 0 || strcmp(string, "<elseBody>") == 0;
}

Tree clearAstDfs(TreeLink root, Tree ast)
{
	int i;
	TreeLink temp;
	 
	for (i = 0; i < root->no_of_children; i++)
	{	
		while (root->children[i]->no_of_children == 1 && ((ASTNodeData*)root->children[i]->data)->symbol->isNonTerminal
			&& !cannotBeDeleted(((ASTNodeData*)root->children[i]->data)->symbol->symbolName))
		{
			temp = root->children[i];
			root->children[i] = root->children[i]->children[0];
			free(temp);
			ast.totalNodes--;
		}
		if (toPush(((ASTNodeData*)root->children[i]->data)->symbol->symbolName))
		{
			*((ASTNodeData*)root->data) = *((ASTNodeData*)root->children[i]->data);
			removeChild(root, i);
			ast.totalLeafNodes--;
			ast.totalNodes--;
			i--;
		}
		else
ast= clearAstDfs(root->children[i],ast);
	}

	return ast;
}

Tree clearAst(Tree ast)
{
	return clearAstDfs(ast.root,ast);
}

void _printAST(TreeLink subtreeRoot)
{
	if (subtreeRoot->no_of_children == 0) return;
	if (((ASTNodeData*)subtreeRoot->data)->symbol->isNonTerminal)
	{
		printf("\n\nNode: %s.\t%d children: ", ((ASTNodeData*)subtreeRoot->data)->symbol->symbolName, subtreeRoot->no_of_children);
	}
	else
	{
		printf("\n\nNode: %s (%s).\t%d children: ", ((ASTNodeData*)subtreeRoot->data)->symbol->symbolName, ((ASTNodeData*)subtreeRoot->data)->tokenInfo.lexeme, subtreeRoot->no_of_children);
	}
	int i;
	for (i = 0; i < subtreeRoot->no_of_children; i++)
	{
		if (((ASTNodeData*)subtreeRoot->children[i]->data)->symbol->isNonTerminal)
		{
			printf(",  %s", ((ASTNodeData*)subtreeRoot->children[i]->data)->symbol->symbolName);
		}
		else
		{
			printf(",  %s (%s)", ((ASTNodeData*)subtreeRoot->children[i]->data)->symbol->symbolName, ((ASTNodeData*)subtreeRoot->children[i]->data)->tokenInfo.lexeme);
		}
	}
	for (i = 0; i < subtreeRoot->no_of_children; i++)
	{
		_printAST(subtreeRoot->children[i]);
	}
}

void printAST(Tree ast)
{
	printf("\n\n\nAST goes here:\n\n");
	_printAST(ast.root);
}

