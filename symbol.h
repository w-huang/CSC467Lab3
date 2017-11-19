#ifndef _SYMBOL_H
#define _SYMBOL_H

#endif

int scopeContainsVariable(char* id);
void addVariable(char* id);
void enterNewScope();
void getVariableDetails(char* id);
void exitScope();