%{
#include "ast.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>

void yyerror(const char* s);
%}

%option yylineno
%option noyywrap

%%

"let"           { return TOKEN_LET; }
"show"          { return TOKEN_SHOW; }
"true"          { return TOKEN_TRUE; }
"false"         { return TOKEN_FALSE; }
"if"            { return TOKEN_IF; }
"else"          { return TOKEN_ELSE; }
"while"         { return TOKEN_WHILE; }
"for"           { return TOKEN_FOR; }
"repeat"        { return TOKEN_REPEAT; }
"times"         { return TOKEN_TIMES; }
"func"          { return TOKEN_FUNC; }
"return"        { return TOKEN_RETURN; }

"=="            { return TOKEN_EQ; }
"!="            { return TOKEN_NEQ; }
">="            { return TOKEN_GTE; }
"<="            { return TOKEN_LTE; }
">"             { return '>'; }
"<"             { return '<'; }

"&&"            { return TOKEN_AND; }
"||"            { return TOKEN_OR; }
"!"             { return '!'; }

"++"            { return TOKEN_INC; }
"--"            { return TOKEN_DEC; }

"+="            { return TOKEN_ADD_ASSIGN; }
"-="            { return TOKEN_SUB_ASSIGN; }
"*="            { return TOKEN_MUL_ASSIGN; }
"/="            { return TOKEN_DIV_ASSIGN; }

"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }
"%"             { return '%'; }
"="             { return '='; }
","             { return ','; }

[0-9]+\.[0-9]+  { yylval.fval = atof(yytext); return TOKEN_FLOAT; }
[0-9]+          { yylval.ival = atoll(yytext); return TOKEN_INT; }

\"([^\"\\]|\\.)*\" {
    yylval.strval = strdup(yytext);
    return TOKEN_STRING;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.strval = strdup(yytext);
    return TOKEN_IDENTIFIER;
}

"("             { return '('; }
")"             { return ')'; }
"{"             { return '{'; }
"}"             { return '}'; }
";"             { return ';'; }

[ \t\r\n]+      { /* ignore whitespace */ }
"//".*          { /* single line comment */ }
"/*"([^*]|(\*+[^*/]))*\*+"/" { /* multi-line comment */ }

.               { yyerror("Unexpected character"); }

%%
