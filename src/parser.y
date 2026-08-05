%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylex();
extern int yylineno;
extern ASTNode* root_ast;

void yyerror(const char* s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
    exit(1);
}
%}

%union {
    long long ival;
    double fval;
    char* strval;
    struct ASTNode* node;
}

%token TOKEN_LET TOKEN_SHOW TOKEN_TRUE TOKEN_FALSE
%token TOKEN_IF TOKEN_ELSE TOKEN_WHILE TOKEN_FOR TOKEN_REPEAT TOKEN_TIMES
%token TOKEN_FUNC TOKEN_RETURN
%token <ival> TOKEN_INT
%token <fval> TOKEN_FLOAT
%token <strval> TOKEN_STRING TOKEN_IDENTIFIER
%token TOKEN_EQ TOKEN_NEQ TOKEN_GTE TOKEN_LTE
%token TOKEN_AND TOKEN_OR
%token TOKEN_INC TOKEN_DEC
%token TOKEN_ADD_ASSIGN TOKEN_SUB_ASSIGN TOKEN_MUL_ASSIGN TOKEN_DIV_ASSIGN

%type <node> program stmts stmt block expr opt_expr
%type <node> param_list arg_list func_decl

%nonassoc LOWER_THAN_ELSE
%nonassoc TOKEN_ELSE

%right '=' TOKEN_ADD_ASSIGN TOKEN_SUB_ASSIGN TOKEN_MUL_ASSIGN TOKEN_DIV_ASSIGN
%left TOKEN_OR
%left TOKEN_AND
%left TOKEN_EQ TOKEN_NEQ
%left '<' '>' TOKEN_LTE TOKEN_GTE
%left '+' '-'
%left '*' '/' '%'
%right '!' UMINUS
%left TOKEN_INC TOKEN_DEC

%%

program:
    stmts {
        root_ast = $1;
    }
    | /* empty */ {
        root_ast = make_program();
    }
    ;

stmts:
    stmt {
        $$ = make_program();
        if ($1) program_add_stmt($$, $1);
    }
    | stmts stmt {
        if ($2) program_add_stmt($1, $2);
        $$ = $1;
    }
    ;

block:
    '{' stmts '}' {
        $$ = make_block();
        for (int i = 0; i < $2->as.program.count; i++) {
            block_add_stmt($$, $2->as.program.stmts[i]);
        }
        free($2->as.program.stmts);
        free($2);
    }
    | '{' '}' {
        $$ = make_block();
    }
    ;

func_decl:
    TOKEN_FUNC TOKEN_IDENTIFIER '(' param_list ')' block {
        $$ = $4; // $4 is already the func_decl node
        free($4->as.func_decl.name);
        $4->as.func_decl.name = strdup($2);
        $4->as.func_decl.body = $6;
        free($2);
    }
    | TOKEN_FUNC TOKEN_IDENTIFIER '(' ')' block {
        $$ = make_func_decl($2);
        $$->as.func_decl.body = $5;
        free($2);
    }
    ;

param_list:
    TOKEN_IDENTIFIER {
        $$ = make_func_decl("");
        func_decl_add_param($$, $1);
        free($1);
    }
    | param_list ',' TOKEN_IDENTIFIER {
        func_decl_add_param($1, $3);
        free($3);
        $$ = $1;
    }
    ;

arg_list:
    expr {
        $$ = make_call("");
        call_add_arg($$, $1);
    }
    | arg_list ',' expr {
        call_add_arg($1, $3);
        $$ = $1;
    }
    ;

stmt:
    TOKEN_LET TOKEN_IDENTIFIER '=' expr ';' { $$ = make_let($2, $4); free($2); }
    | TOKEN_IDENTIFIER '=' expr ';' { $$ = make_assign($1, $3); free($1); }
    | TOKEN_IDENTIFIER TOKEN_ADD_ASSIGN expr ';' { $$ = make_compound_assign(BINOP_ADD, $1, $3); free($1); }
    | TOKEN_IDENTIFIER TOKEN_SUB_ASSIGN expr ';' { $$ = make_compound_assign(BINOP_SUB, $1, $3); free($1); }
    | TOKEN_IDENTIFIER TOKEN_MUL_ASSIGN expr ';' { $$ = make_compound_assign(BINOP_MUL, $1, $3); free($1); }
    | TOKEN_IDENTIFIER TOKEN_DIV_ASSIGN expr ';' { $$ = make_compound_assign(BINOP_DIV, $1, $3); free($1); }
    | TOKEN_IDENTIFIER TOKEN_INC ';' { $$ = make_postfix(POSTFIX_INC, $1); free($1); }
    | TOKEN_IDENTIFIER TOKEN_DEC ';' { $$ = make_postfix(POSTFIX_DEC, $1); free($1); }
    | TOKEN_SHOW '(' expr ')' ';' { $$ = make_show($3); }
    | expr ';' { $$ = $1; }
    | block { $$ = $1; }
    | TOKEN_IF '(' expr ')' stmt %prec LOWER_THAN_ELSE { $$ = make_if($3, $5, NULL); }
    | TOKEN_IF '(' expr ')' stmt TOKEN_ELSE stmt { $$ = make_if($3, $5, $7); }
    | TOKEN_WHILE '(' expr ')' stmt { $$ = make_while($3, $5); }
    | TOKEN_FOR '(' stmt opt_expr ';' opt_expr ')' stmt { $$ = make_for($3, $4, $6, $8); }
    | TOKEN_REPEAT expr TOKEN_TIMES stmt { $$ = make_repeat($2, $4); }
    | func_decl { $$ = $1; }
    | TOKEN_RETURN opt_expr ';' { $$ = make_return($2); }
    ;

opt_expr:
    expr { $$ = $1; }
    | /* empty */ { $$ = NULL; }
    ;

expr:
    TOKEN_INT { $$ = make_int_literal($1); }
    | TOKEN_FLOAT { $$ = make_float_literal($1); }
    | TOKEN_TRUE { $$ = make_bool_literal(1); }
    | TOKEN_FALSE { $$ = make_bool_literal(0); }
    | TOKEN_STRING {
        char* str = $1;
        int len = strlen(str);
        char* inner = malloc(len - 1);
        strncpy(inner, str + 1, len - 2);
        inner[len - 2] = '\0';
        $$ = make_string_literal(inner);
        free(inner);
        free($1);
    }
    | TOKEN_IDENTIFIER { $$ = make_identifier($1); free($1); }
    | TOKEN_IDENTIFIER '(' arg_list ')' {
        $$ = $3;
        free($$->as.call_expr.name);
        $$->as.call_expr.name = strdup($1);
        free($1);
    }
    | TOKEN_IDENTIFIER '(' ')' {
        $$ = make_call($1);
        free($1);
    }
    | TOKEN_IDENTIFIER '=' expr { $$ = make_assign($1, $3); free($1); }
    | TOKEN_IDENTIFIER TOKEN_INC { $$ = make_postfix(POSTFIX_INC, $1); free($1); }
    | TOKEN_IDENTIFIER TOKEN_DEC { $$ = make_postfix(POSTFIX_DEC, $1); free($1); }
    | expr '+' expr { $$ = make_binop(BINOP_ADD, $1, $3); }
    | expr '-' expr { $$ = make_binop(BINOP_SUB, $1, $3); }
    | expr '*' expr { $$ = make_binop(BINOP_MUL, $1, $3); }
    | expr '/' expr { $$ = make_binop(BINOP_DIV, $1, $3); }
    | expr '%' expr { $$ = make_binop(BINOP_MOD, $1, $3); }
    | expr '>' expr { $$ = make_binop(BINOP_GT, $1, $3); }
    | expr '<' expr { $$ = make_binop(BINOP_LT, $1, $3); }
    | expr TOKEN_GTE expr { $$ = make_binop(BINOP_GTE, $1, $3); }
    | expr TOKEN_LTE expr { $$ = make_binop(BINOP_LTE, $1, $3); }
    | expr TOKEN_EQ expr { $$ = make_binop(BINOP_EQ, $1, $3); }
    | expr TOKEN_NEQ expr { $$ = make_binop(BINOP_NEQ, $1, $3); }
    | expr TOKEN_AND expr { $$ = make_binop(BINOP_AND, $1, $3); }
    | expr TOKEN_OR expr { $$ = make_binop(BINOP_OR, $1, $3); }
    | '-' expr %prec UMINUS { $$ = make_unaryop(UNARY_MINUS, $2); }
    | '!' expr { $$ = make_unaryop(UNARY_NOT, $2); }
    | '(' expr ')' { $$ = $2; }
    ;

%%
