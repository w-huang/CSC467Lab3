#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "symbol.h"

int scopeContainsVariable(char* id){
    return -1;
}
void addVariable(char* id){

}
void enterNewScope() {

}

symbol getVariableDetails(char* id){
    symbol x;
    memset(&x, 0, sizeof(symbol));
    return x;
}

void exitScope(){

}