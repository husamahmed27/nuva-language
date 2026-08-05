#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"

typedef enum {
    VAL_INT,
    VAL_FLOAT,
    VAL_STRING,
    VAL_BOOL,
    VAL_NULL
} ValueType;

typedef struct {
    ValueType type;
    union {
        long long i;
        double f;
        char* s;
        int b;
    } as;
} Value;

void interpret(ASTNode* root);
Value evaluate(ASTNode* node);
void print_value(Value val);

#endif // INTERPRETER_H
