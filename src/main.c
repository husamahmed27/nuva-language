#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "interpreter.h"

extern FILE* yyin;
extern int yyparse();

ASTNode* root_ast = NULL;

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename.nv>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    yyin = file;
    if (yyparse() == 0) {
        // Parsing successful, run interpreter
        interpret(root_ast);
    }

    fclose(file);
    free_ast(root_ast);
    return 0;
}
