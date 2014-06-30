#ifndef _CODEGENERATOR_H_
#define _CODEGENERATOR_H_

#include <stdio.h>
#include "Tree.h"

void generateCode( Tree ast,FILE* asmm);
void _generateCode(TreeLink root, FILE* asmm);
void initRegisters();
int getEmptyRegister();
void declarationInfo(TreeLink root, FILE* asmm);
void initBasicIo(fin, asmm);

#endif