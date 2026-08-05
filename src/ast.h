#ifndef AST_H
#define AST_H

typedef enum {
    AST_PROGRAM,
    AST_BLOCK,
    AST_SHOW,
    AST_LET,
    AST_ASSIGN,
    AST_COMPOUND_ASSIGN,
    AST_IF,
    AST_WHILE,
    AST_FOR,
    AST_REPEAT,
    AST_FUNC_DECL,
    AST_RETURN,
    AST_CALL,
    AST_INT_LITERAL,
    AST_FLOAT_LITERAL,
    AST_STRING_LITERAL,
    AST_BOOL_LITERAL,
    AST_IDENTIFIER,
    AST_BINOP,
    AST_UNARYOP,
    AST_POSTFIX
} ASTNodeType;

typedef enum {
    BINOP_ADD, BINOP_SUB, BINOP_MUL, BINOP_DIV, BINOP_MOD,
    BINOP_GT, BINOP_LT, BINOP_GTE, BINOP_LTE, BINOP_EQ, BINOP_NEQ,
    BINOP_AND, BINOP_OR
} BinOpType;

typedef enum {
    UNARY_MINUS, UNARY_NOT
} UnaryOpType;

typedef enum {
    POSTFIX_INC, POSTFIX_DEC
} PostfixOpType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct {
            struct ASTNode** stmts;
            int count;
            int capacity;
        } program;
        struct {
            struct ASTNode** stmts;
            int count;
            int capacity;
        } block;
        struct {
            struct ASTNode* expr;
        } show;
        struct {
            char* name;
            struct ASTNode* expr;
        } let_decl;
        struct {
            char* name;
            struct ASTNode* expr;
        } assign;
        struct {
            BinOpType op;
            char* name;
            struct ASTNode* expr;
        } compound_assign;
        struct {
            struct ASTNode* cond;
            struct ASTNode* then_branch;
            struct ASTNode* else_branch;
        } if_stmt;
        struct {
            struct ASTNode* cond;
            struct ASTNode* body;
        } while_stmt;
        struct {
            struct ASTNode* init;
            struct ASTNode* cond;
            struct ASTNode* inc;
            struct ASTNode* body;
        } for_stmt;
        struct {
            struct ASTNode* count_expr;
            struct ASTNode* body;
        } repeat_stmt;
        struct {
            char* name;
            char** params;
            int param_count;
            int param_capacity;
            struct ASTNode* body;
        } func_decl;
        struct {
            struct ASTNode* expr;
        } return_stmt;
        struct {
            char* name;
            struct ASTNode** args;
            int arg_count;
            int arg_capacity;
        } call_expr;
        struct {
            long long value;
        } int_literal;
        struct {
            double value;
        } float_literal;
        struct {
            char* value;
        } string_literal;
        struct {
            int value; // 0 or 1
        } bool_literal;
        struct {
            char* name;
        } identifier;
        struct {
            BinOpType op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binop;
        struct {
            UnaryOpType op;
            struct ASTNode* expr;
        } unaryop;
        struct {
            PostfixOpType op;
            char* name;
        } postfix;
    } as;
} ASTNode;

ASTNode* make_program();
void program_add_stmt(ASTNode* prog, ASTNode* stmt);
ASTNode* make_block();
void block_add_stmt(ASTNode* block, ASTNode* stmt);
ASTNode* make_show(ASTNode* expr);
ASTNode* make_let(const char* name, ASTNode* expr);
ASTNode* make_assign(const char* name, ASTNode* expr);
ASTNode* make_compound_assign(BinOpType op, const char* name, ASTNode* expr);
ASTNode* make_if(ASTNode* cond, ASTNode* then_branch, ASTNode* else_branch);
ASTNode* make_while(ASTNode* cond, ASTNode* body);
ASTNode* make_for(ASTNode* init, ASTNode* cond, ASTNode* inc, ASTNode* body);
ASTNode* make_repeat(ASTNode* count_expr, ASTNode* body);
ASTNode* make_func_decl(const char* name);
void func_decl_add_param(ASTNode* func, const char* param);
ASTNode* make_return(ASTNode* expr);
ASTNode* make_call(const char* name);
void call_add_arg(ASTNode* call, ASTNode* arg);
ASTNode* make_int_literal(long long val);
ASTNode* make_float_literal(double val);
ASTNode* make_string_literal(const char* val);
ASTNode* make_bool_literal(int val);
ASTNode* make_identifier(const char* name);
ASTNode* make_binop(BinOpType op, ASTNode* left, ASTNode* right);
ASTNode* make_unaryop(UnaryOpType op, ASTNode* expr);
ASTNode* make_postfix(PostfixOpType op, const char* name);
void free_ast(ASTNode* node);

#endif // AST_H
