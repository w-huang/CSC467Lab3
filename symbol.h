#ifndef _SYMBOL_H
#define _SYMBOL_H

#endif

struct symbolEntry;
typedef struct symbolEntry symbol;

struct symbolEntry{
    int type;
    int intValue;
    float floatValue;
    int boolValue;
    int ivec[4];
    int bvec[4];
    float vec[4];
    int length; //1 means it's not a vec
};

int scopeContainsVariable(char* id);
void addVariable(char* id);
void enterNewScope();
symbol getVariableDetails(char* id);
void exitScope();