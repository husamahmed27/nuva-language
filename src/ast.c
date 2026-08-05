#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode* make_program() {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_PROGRAM;
    node->as.program.count = 0;
    node->as.program.capacity = 8;
    node->as.program.stmts = malloc(sizeof(ASTNode*) * node->as.program.capacity);
    return node;
}

void program_add_stmt(ASTNode* prog, ASTNode* stmt) {
    if (prog->as.program.count >= prog->as.program.capacity) {
        prog->as.program.capacity *= 2;
        prog->as.program.stmts = realloc(prog->as.program.stmts, sizeof(ASTNode*) * prog->as.program.capacity);
    }
    prog->as.program.stmts[prog->as.program.count++] = stmt;
}

ASTNode* make_block() {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BLOCK;
    node->as.block.count = 0;
    node->as.block.capacity = 8;
    node->as.block.stmts = malloc(sizeof(ASTNode*) * node->as.block.capacity);
    return node;
}

void block_add_stmt(ASTNode* block, ASTNode* stmt) {
    if (block->as.block.count >= block->as.block.capacity) {
        block->as.block.capacity *= 2;
        block->as.block.stmts = realloc(block->as.block.stmts, sizeof(ASTNode*) * block->as.block.capacity);
    }
    block->as.block.stmts[block->as.block.count++] = stmt;
}

ASTNode* make_show(ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_SHOW;
    node->as.show.expr = expr;
    return node;
}

ASTNode* make_let(const char* name, ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_LET;
    node->as.let_decl.name = strdup(name);
    node->as.let_decl.expr = expr;
    return node;
}

ASTNode* make_assign(const char* name, ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGN;
    node->as.assign.name = strdup(name);
    node->as.assign.expr = expr;
    return node;
}

ASTNode* make_compound_assign(BinOpType op, const char* name, ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_COMPOUND_ASSIGN;
    node->as.compound_assign.op = op;
    node->as.compound_assign.name = strdup(name);
    node->as.compound_assign.expr = expr;
    return node;
}

ASTNode* make_if(ASTNode* cond, ASTNode* then_branch, ASTNode* else_branch) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IF;
    node->as.if_stmt.cond = cond;
    node->as.if_stmt.then_branch = then_branch;
    node->as.if_stmt.else_branch = else_branch;
    return node;
}

ASTNode* make_while(ASTNode* cond, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_WHILE;
    node->as.while_stmt.cond = cond;
    node->as.while_stmt.body = body;
    return node;
}

ASTNode* make_for(ASTNode* init, ASTNode* cond, ASTNode* inc, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_FOR;
    node->as.for_stmt.init = init;
    node->as.for_stmt.cond = cond;
    node->as.for_stmt.inc = inc;
    node->as.for_stmt.body = body;
    return node;
}

ASTNode* make_repeat(ASTNode* count_expr, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_REPEAT;
    node->as.repeat_stmt.count_expr = count_expr;
    node->as.repeat_stmt.body = body;
    return node;
}

ASTNode* make_func_decl(const char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_FUNC_DECL;
    node->as.func_decl.name = strdup(name);
    node->as.func_decl.param_count = 0;
    node->as.func_decl.param_capacity = 4;
    node->as.func_decl.params = malloc(sizeof(char*) * 4);
    node->as.func_decl.body = NULL;
    return node;
}

void func_decl_add_param(ASTNode* func, const char* param) {
    if (func->as.func_decl.param_count >= func->as.func_decl.param_capacity) {
        func->as.func_decl.param_capacity *= 2;
        func->as.func_decl.params = realloc(func->as.func_decl.params, sizeof(char*) * func->as.func_decl.param_capacity);
    }
    func->as.func_decl.params[func->as.func_decl.param_count++] = strdup(param);
}

ASTNode* make_return(ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_RETURN;
    node->as.return_stmt.expr = expr;
    return node;
}

ASTNode* make_call(const char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_CALL;
    node->as.call_expr.name = strdup(name);
    node->as.call_expr.arg_count = 0;
    node->as.call_expr.arg_capacity = 4;
    node->as.call_expr.args = malloc(sizeof(ASTNode*) * 4);
    return node;
}

void call_add_arg(ASTNode* call, ASTNode* arg) {
    if (call->as.call_expr.arg_count >= call->as.call_expr.arg_capacity) {
        call->as.call_expr.arg_capacity *= 2;
        call->as.call_expr.args = realloc(call->as.call_expr.args, sizeof(ASTNode*) * call->as.call_expr.arg_capacity);
    }
    call->as.call_expr.args[call->as.call_expr.arg_count++] = arg;
}


ASTNode* make_int_literal(long long val) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_INT_LITERAL;
    node->as.int_literal.value = val;
    return node;
}

ASTNode* make_float_literal(double val) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_FLOAT_LITERAL;
    node->as.float_literal.value = val;
    return node;
}

ASTNode* make_string_literal(const char* val) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_STRING_LITERAL;
    node->as.string_literal.value = strdup(val);
    return node;
}

ASTNode* make_bool_literal(int val) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BOOL_LITERAL;
    node->as.bool_literal.value = val;
    return node;
}

ASTNode* make_identifier(const char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IDENTIFIER;
    node->as.identifier.name = strdup(name);
    return node;
}

ASTNode* make_binop(BinOpType op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BINOP;
    node->as.binop.op = op;
    node->as.binop.left = left;
    node->as.binop.right = right;
    return node;
}

ASTNode* make_unaryop(UnaryOpType op, ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_UNARYOP;
    node->as.unaryop.op = op;
    node->as.unaryop.expr = expr;
    return node;
}

ASTNode* make_postfix(PostfixOpType op, const char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_POSTFIX;
    node->as.postfix.op = op;
    node->as.postfix.name = strdup(name);
    return node;
}

void free_ast(ASTNode* node) {
    if (!node) return;
    switch (node->type) {
        case AST_PROGRAM:
            for (int i = 0; i < node->as.program.count; i++) free_ast(node->as.program.stmts[i]);
            free(node->as.program.stmts);
            break;
        case AST_BLOCK:
            for (int i = 0; i < node->as.block.count; i++) free_ast(node->as.block.stmts[i]);
            free(node->as.block.stmts);
            break;
        case AST_SHOW:
            free_ast(node->as.show.expr);
            break;
        case AST_LET:
            free(node->as.let_decl.name);
            free_ast(node->as.let_decl.expr);
            break;
        case AST_ASSIGN:
            free(node->as.assign.name);
            free_ast(node->as.assign.expr);
            break;
        case AST_COMPOUND_ASSIGN:
            free(node->as.compound_assign.name);
            free_ast(node->as.compound_assign.expr);
            break;
        case AST_IF:
            free_ast(node->as.if_stmt.cond);
            free_ast(node->as.if_stmt.then_branch);
            free_ast(node->as.if_stmt.else_branch);
            break;
        case AST_WHILE:
            free_ast(node->as.while_stmt.cond);
            free_ast(node->as.while_stmt.body);
            break;
        case AST_FOR:
            free_ast(node->as.for_stmt.init);
            free_ast(node->as.for_stmt.cond);
            free_ast(node->as.for_stmt.inc);
            free_ast(node->as.for_stmt.body);
            break;
        case AST_REPEAT:
            free_ast(node->as.repeat_stmt.count_expr);
            free_ast(node->as.repeat_stmt.body);
            break;
        case AST_FUNC_DECL:
            free(node->as.func_decl.name);
            for (int i = 0; i < node->as.func_decl.param_count; i++) free(node->as.func_decl.params[i]);
            free(node->as.func_decl.params);
            free_ast(node->as.func_decl.body);
            break;
        case AST_RETURN:
            free_ast(node->as.return_stmt.expr);
            break;
        case AST_CALL:
            free(node->as.call_expr.name);
            for (int i = 0; i < node->as.call_expr.arg_count; i++) free_ast(node->as.call_expr.args[i]);
            free(node->as.call_expr.args);
            break;
        case AST_STRING_LITERAL:
            free(node->as.string_literal.value);
            break;
        case AST_IDENTIFIER:
            free(node->as.identifier.name);
            break;
        case AST_BINOP:
            free_ast(node->as.binop.left);
            free_ast(node->as.binop.right);
            break;
        case AST_UNARYOP:
            free_ast(node->as.unaryop.expr);
            break;
        case AST_POSTFIX:
            free(node->as.postfix.name);
            break;
        case AST_INT_LITERAL:
        case AST_FLOAT_LITERAL:
        case AST_BOOL_LITERAL:
            break;
    }
    free(node);
}
