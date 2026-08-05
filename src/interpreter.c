#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

typedef struct EnvEntry {
    char* name;
    Value val;
    struct EnvEntry* next;
} EnvEntry;

typedef struct Env {
    EnvEntry* entries;
    struct Env* parent;
} Env;

Env* current_env = NULL;

typedef struct FuncEntry {
    char* name;
    ASTNode* decl; // AST_FUNC_DECL node
    struct FuncEntry* next;
} FuncEntry;

FuncEntry* func_table = NULL;

int is_returning = 0;
Value return_value;

void push_env() {
    Env* env = malloc(sizeof(Env));
    env->entries = NULL;
    env->parent = current_env;
    current_env = env;
}

void pop_env() {
    if (!current_env) return;
    EnvEntry* curr = current_env->entries;
    while (curr) {
        EnvEntry* next = curr->next;
        free(curr->name);
        if (curr->val.type == VAL_STRING) free(curr->val.as.s);
        free(curr);
        curr = next;
    }
    Env* parent = current_env->parent;
    free(current_env);
    current_env = parent;
}

void define_var(const char* name, Value val) {
    EnvEntry* entry = malloc(sizeof(EnvEntry));
    entry->name = strdup(name);
    if (val.type == VAL_STRING) {
        entry->val.type = VAL_STRING;
        entry->val.as.s = strdup(val.as.s);
    } else {
        entry->val = val;
    }
    entry->next = current_env->entries;
    current_env->entries = entry;
}

void assign_var(const char* name, Value val) {
    Env* env = current_env;
    while (env) {
        EnvEntry* curr = env->entries;
        while (curr) {
            if (strcmp(curr->name, name) == 0) {
                if (curr->val.type == VAL_STRING) free(curr->val.as.s);
                if (val.type == VAL_STRING) {
                    curr->val.type = VAL_STRING;
                    curr->val.as.s = strdup(val.as.s);
                } else {
                    curr->val = val;
                }
                return;
            }
            curr = curr->next;
        }
        env = env->parent;
    }
    fprintf(stderr, "Runtime Error: Undefined variable '%s' for assignment\n", name);
    exit(1);
}

Value get_var(const char* name) {
    Env* env = current_env;
    while (env) {
        EnvEntry* curr = env->entries;
        while (curr) {
            if (strcmp(curr->name, name) == 0) {
                Value val = curr->val;
                if (val.type == VAL_STRING) {
                    Value copy; copy.type = VAL_STRING; copy.as.s = strdup(val.as.s); return copy;
                }
                return val;
            }
            curr = curr->next;
        }
        env = env->parent;
    }
    fprintf(stderr, "Runtime Error: Undefined variable '%s'\n", name);
    exit(1);
}

void register_func(ASTNode* decl) {
    FuncEntry* entry = malloc(sizeof(FuncEntry));
    entry->name = strdup(decl->as.func_decl.name);
    entry->decl = decl;
    entry->next = func_table;
    func_table = entry;
}

ASTNode* get_func(const char* name) {
    FuncEntry* curr = func_table;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr->decl;
        curr = curr->next;
    }
    return NULL;
}

void print_value(Value val) {
    switch (val.type) {
        case VAL_INT: printf("%" PRId64, val.as.i); break;
        case VAL_FLOAT: printf("%g", val.as.f); break;
        case VAL_STRING: printf("%s", val.as.s); break;
        case VAL_BOOL: printf("%s", val.as.b ? "true" : "false"); break;
        case VAL_NULL: printf("null"); break;
    }
}

void free_val(Value val) {
    if (val.type == VAL_STRING) free(val.as.s);
}

Value to_string_val(Value val) {
    if (val.type == VAL_STRING) {
        Value copy; copy.type = VAL_STRING; copy.as.s = strdup(val.as.s); return copy;
    }
    char buf[128];
    if (val.type == VAL_INT) snprintf(buf, sizeof(buf), "%" PRId64, val.as.i);
    else if (val.type == VAL_FLOAT) snprintf(buf, sizeof(buf), "%g", val.as.f);
    else if (val.type == VAL_BOOL) snprintf(buf, sizeof(buf), "%s", val.as.b ? "true" : "false");
    else if (val.type == VAL_NULL) snprintf(buf, sizeof(buf), "null");
    Value res; res.type = VAL_STRING; res.as.s = strdup(buf);
    return res;
}

int is_truthy(Value val) {
    if (val.type == VAL_BOOL) return val.as.b;
    if (val.type == VAL_INT) return val.as.i != 0;
    if (val.type == VAL_FLOAT) return val.as.f != 0.0;
    if (val.type == VAL_STRING) return strlen(val.as.s) > 0;
    return 0; // null
}

Value evaluate_binop(BinOpType op, Value left, Value right) {
    Value res; res.type = VAL_NULL; res.as.i = 0;
    if (op == BINOP_ADD && (left.type == VAL_STRING || right.type == VAL_STRING)) {
        Value ls = to_string_val(left);
        Value rs = to_string_val(right);
        int len = strlen(ls.as.s) + strlen(rs.as.s) + 1;
        char* buf = malloc(len);
        strcpy(buf, ls.as.s);
        strcat(buf, rs.as.s);
        res.type = VAL_STRING;
        res.as.s = buf;
        free_val(ls); free_val(rs);
        return res;
    }

    if (left.type == VAL_INT && right.type == VAL_INT) {
        long long a = left.as.i; long long b = right.as.i;
        switch (op) {
            case BINOP_ADD: res.type = VAL_INT; res.as.i = a + b; break;
            case BINOP_SUB: res.type = VAL_INT; res.as.i = a - b; break;
            case BINOP_MUL: res.type = VAL_INT; res.as.i = a * b; break;
            case BINOP_DIV: if(b==0){fprintf(stderr, "Division by zero\n");exit(1);} res.type = VAL_INT; res.as.i = a / b; break;
            case BINOP_MOD: if(b==0){fprintf(stderr, "Division by zero\n");exit(1);} res.type = VAL_INT; res.as.i = a % b; break;
            case BINOP_GT: res.type = VAL_BOOL; res.as.b = a > b; break;
            case BINOP_LT: res.type = VAL_BOOL; res.as.b = a < b; break;
            case BINOP_GTE: res.type = VAL_BOOL; res.as.b = a >= b; break;
            case BINOP_LTE: res.type = VAL_BOOL; res.as.b = a <= b; break;
            case BINOP_EQ: res.type = VAL_BOOL; res.as.b = a == b; break;
            case BINOP_NEQ: res.type = VAL_BOOL; res.as.b = a != b; break;
            default: break;
        }
    } else if ((left.type == VAL_INT || left.type == VAL_FLOAT) && (right.type == VAL_INT || right.type == VAL_FLOAT)) {
        double a = left.type == VAL_INT ? left.as.i : left.as.f;
        double b = right.type == VAL_INT ? right.as.i : right.as.f;
        switch (op) {
            case BINOP_ADD: res.type = VAL_FLOAT; res.as.f = a + b; break;
            case BINOP_SUB: res.type = VAL_FLOAT; res.as.f = a - b; break;
            case BINOP_MUL: res.type = VAL_FLOAT; res.as.f = a * b; break;
            case BINOP_DIV: res.type = VAL_FLOAT; res.as.f = a / b; break;
            case BINOP_GT: res.type = VAL_BOOL; res.as.b = a > b; break;
            case BINOP_LT: res.type = VAL_BOOL; res.as.b = a < b; break;
            case BINOP_GTE: res.type = VAL_BOOL; res.as.b = a >= b; break;
            case BINOP_LTE: res.type = VAL_BOOL; res.as.b = a <= b; break;
            case BINOP_EQ: res.type = VAL_BOOL; res.as.b = a == b; break;
            case BINOP_NEQ: res.type = VAL_BOOL; res.as.b = a != b; break;
            default: fprintf(stderr, "Invalid float operator\n"); exit(1);
        }
    } else if (left.type == VAL_BOOL && right.type == VAL_BOOL) {
        int a = left.as.b; int b = right.as.b;
        if (op == BINOP_AND) { res.type = VAL_BOOL; res.as.b = a && b; }
        else if (op == BINOP_OR) { res.type = VAL_BOOL; res.as.b = a || b; }
        else if (op == BINOP_EQ) { res.type = VAL_BOOL; res.as.b = a == b; }
        else if (op == BINOP_NEQ) { res.type = VAL_BOOL; res.as.b = a != b; }
        else { fprintf(stderr, "Invalid boolean operator\n"); exit(1); }
    } else if (left.type == VAL_STRING && right.type == VAL_STRING) {
        if (op == BINOP_EQ) { res.type = VAL_BOOL; res.as.b = strcmp(left.as.s, right.as.s) == 0; }
        else if (op == BINOP_NEQ) { res.type = VAL_BOOL; res.as.b = strcmp(left.as.s, right.as.s) != 0; }
        else { fprintf(stderr, "Invalid string operator\n"); exit(1); }
    } else {
        fprintf(stderr, "Type mismatch in binary operation\n"); exit(1);
    }
    return res;
}

Value evaluate(ASTNode* node) {
    Value null_val = {VAL_NULL, {0}};
    if (!node) return null_val;

    if (is_returning && node->type != AST_PROGRAM && node->type != AST_BLOCK) {
        return null_val;
    }

    switch (node->type) {
        case AST_PROGRAM: {
            Value last_val = null_val;
            for (int i = 0; i < node->as.program.count; i++) {
                if (is_returning) break;
                free_val(last_val);
                last_val = evaluate(node->as.program.stmts[i]);
            }
            return last_val;
        }
        case AST_BLOCK: {
            push_env();
            Value last_val = null_val;
            for (int i = 0; i < node->as.block.count; i++) {
                if (is_returning) break;
                free_val(last_val);
                last_val = evaluate(node->as.block.stmts[i]);
            }
            pop_env();
            return last_val;
        }
        case AST_SHOW: {
            Value val = evaluate(node->as.show.expr);
            print_value(val);
            printf("\n");
            free_val(val);
            return null_val;
        }
        case AST_LET: {
            Value val = evaluate(node->as.let_decl.expr);
            define_var(node->as.let_decl.name, val);
            free_val(val);
            return null_val;
        }
        case AST_ASSIGN: {
            Value val = evaluate(node->as.assign.expr);
            assign_var(node->as.assign.name, val);
            Value copy;
            if (val.type == VAL_STRING) { copy.type = VAL_STRING; copy.as.s = strdup(val.as.s); }
            else { copy = val; }
            free_val(val);
            return copy;
        }
        case AST_COMPOUND_ASSIGN: {
            Value current = get_var(node->as.compound_assign.name);
            Value rhs = evaluate(node->as.compound_assign.expr);
            Value new_val = evaluate_binop(node->as.compound_assign.op, current, rhs);
            assign_var(node->as.compound_assign.name, new_val);
            free_val(current); free_val(rhs);
            return new_val;
        }
        case AST_IF: {
            Value cond = evaluate(node->as.if_stmt.cond);
            if (is_truthy(cond)) {
                free_val(cond);
                return evaluate(node->as.if_stmt.then_branch);
            } else {
                free_val(cond);
                if (node->as.if_stmt.else_branch) {
                    return evaluate(node->as.if_stmt.else_branch);
                }
            }
            return null_val;
        }
        case AST_WHILE: {
            Value cond = evaluate(node->as.while_stmt.cond);
            while (is_truthy(cond)) {
                free_val(cond);
                Value body_res = evaluate(node->as.while_stmt.body);
                free_val(body_res);
                if (is_returning) break;
                cond = evaluate(node->as.while_stmt.cond);
            }
            if (!is_returning) free_val(cond);
            return null_val;
        }
        case AST_FOR: {
            push_env(); // Create scope for the initialization variable
            Value init = evaluate(node->as.for_stmt.init);
            free_val(init);
            
            Value cond = evaluate(node->as.for_stmt.cond);
            while (node->as.for_stmt.cond == NULL || is_truthy(cond)) {
                free_val(cond);
                Value body_res = evaluate(node->as.for_stmt.body);
                free_val(body_res);
                if (is_returning) break;
                
                Value inc = evaluate(node->as.for_stmt.inc);
                free_val(inc);
                
                cond = evaluate(node->as.for_stmt.cond);
            }
            if (!is_returning && node->as.for_stmt.cond != NULL) free_val(cond);
            pop_env();
            return null_val;
        }
        case AST_REPEAT: {
            Value count_val = evaluate(node->as.repeat_stmt.count_expr);
            long long count = 0;
            if (count_val.type == VAL_INT) count = count_val.as.i;
            else if (count_val.type == VAL_FLOAT) count = (long long)count_val.as.f;
            free_val(count_val);
            
            for (long long i = 0; i < count; i++) {
                Value body_res = evaluate(node->as.repeat_stmt.body);
                free_val(body_res);
                if (is_returning) break;
            }
            return null_val;
        }
        case AST_FUNC_DECL: {
            register_func(node);
            return null_val;
        }
        case AST_RETURN: {
            if (node->as.return_stmt.expr) {
                return_value = evaluate(node->as.return_stmt.expr);
            } else {
                return_value = null_val;
            }
            is_returning = 1;
            return null_val;
        }
        case AST_CALL: {
            const char* name = node->as.call_expr.name;
            int arg_count = node->as.call_expr.arg_count;
            
            // Handle built-ins
            if (strcmp(name, "ask") == 0) {
                if (arg_count > 0) {
                    Value prompt = evaluate(node->as.call_expr.args[0]);
                    print_value(prompt);
                    free_val(prompt);
                }
                char buf[1024];
                if (fgets(buf, sizeof(buf), stdin)) {
                    buf[strcspn(buf, "\r\n")] = 0; // remove newline
                    Value v; v.type = VAL_STRING; v.as.s = strdup(buf);
                    return v;
                } else {
                    exit(0); // Gracefully exit on EOF
                }
                return null_val;
            } else if (strcmp(name, "toNumber") == 0) {
                if (arg_count != 1) { fprintf(stderr, "toNumber expects 1 argument\n"); exit(1); }
                Value arg = evaluate(node->as.call_expr.args[0]);
                Value res = null_val;
                if (arg.type == VAL_STRING) {
                    if (strchr(arg.as.s, '.')) {
                        res.type = VAL_FLOAT; res.as.f = atof(arg.as.s);
                    } else {
                        res.type = VAL_INT; res.as.i = atoll(arg.as.s);
                    }
                } else if (arg.type == VAL_INT || arg.type == VAL_FLOAT) {
                    res = arg;
                    if (arg.type == VAL_STRING) { // unreachable but keeps logic clear if deep copy was needed
                    }
                } else {
                    fprintf(stderr, "toNumber expects string/number\n"); exit(1);
                }
                if (arg.type == VAL_STRING) free_val(arg);
                return res;
            } else if (strcmp(name, "random") == 0) {
                if (arg_count != 2) { fprintf(stderr, "random expects 2 arguments (min, max)\n"); exit(1); }
                Value vmin = evaluate(node->as.call_expr.args[0]);
                Value vmax = evaluate(node->as.call_expr.args[1]);
                if (vmin.type != VAL_INT || vmax.type != VAL_INT) { fprintf(stderr, "random expects integers\n"); exit(1); }
                long long min = vmin.as.i; long long max = vmax.as.i;
                long long r = min + (rand() % (max - min + 1));
                free_val(vmin); free_val(vmax);
                Value res; res.type = VAL_INT; res.as.i = r;
                return res;
            }

            // User defined functions
            ASTNode* func = get_func(name);
            if (!func) {
                fprintf(stderr, "Runtime Error: Undefined function '%s'\n", name);
                exit(1);
            }
            if (arg_count != func->as.func_decl.param_count) {
                fprintf(stderr, "Runtime Error: Function '%s' expects %d arguments, got %d\n", name, func->as.func_decl.param_count, arg_count);
                exit(1);
            }
            
            // Evaluate arguments in current scope
            Value* evaled_args = malloc(sizeof(Value) * arg_count);
            for (int i = 0; i < arg_count; i++) {
                evaled_args[i] = evaluate(node->as.call_expr.args[i]);
            }
            
            // Push new scope for function (functions are technically lexically scoped to globals here, 
            // but we're keeping simple dynamic scoping via environment stack, which is typical for such toy interpreters 
            // unless closures are required. We'll push a fresh environment linked to global).
            // Actually, for proper scope, we link to the global env.
            // But we can just push on top of the current env to allow dynamic scoping (simpler),
            // or find the global env.
            // Let's implement static scoping to global:
            Env* saved_env = current_env;
            // Find global env
            Env* global_env = current_env;
            while (global_env->parent) global_env = global_env->parent;
            
            Env* func_env = malloc(sizeof(Env));
            func_env->entries = NULL;
            func_env->parent = global_env;
            current_env = func_env;
            
            for (int i = 0; i < arg_count; i++) {
                define_var(func->as.func_decl.params[i], evaled_args[i]);
                free_val(evaled_args[i]);
            }
            free(evaled_args);
            
            // Execute body
            evaluate(func->as.func_decl.body);
            
            Value res = null_val;
            if (is_returning) {
                res = return_value;
                is_returning = 0;
            }
            
            pop_env();
            current_env = saved_env; // restore
            
            return res;
        }
        case AST_INT_LITERAL: {
            Value val; val.type = VAL_INT; val.as.i = node->as.int_literal.value; return val;
        }
        case AST_FLOAT_LITERAL: {
            Value val; val.type = VAL_FLOAT; val.as.f = node->as.float_literal.value; return val;
        }
        case AST_STRING_LITERAL: {
            Value val; val.type = VAL_STRING; val.as.s = strdup(node->as.string_literal.value); return val;
        }
        case AST_BOOL_LITERAL: {
            Value val; val.type = VAL_BOOL; val.as.b = node->as.bool_literal.value; return val;
        }
        case AST_IDENTIFIER: {
            return get_var(node->as.identifier.name);
        }
        case AST_BINOP: {
            Value left = evaluate(node->as.binop.left);
            if (node->as.binop.op == BINOP_AND) {
                if (!is_truthy(left)) { return left; }
                free_val(left);
                return evaluate(node->as.binop.right);
            }
            if (node->as.binop.op == BINOP_OR) {
                if (is_truthy(left)) { return left; }
                free_val(left);
                return evaluate(node->as.binop.right);
            }
            Value right = evaluate(node->as.binop.right);
            Value res = evaluate_binop(node->as.binop.op, left, right);
            free_val(left); free_val(right);
            return res;
        }
        case AST_UNARYOP: {
            Value expr = evaluate(node->as.unaryop.expr);
            Value res; res.type = VAL_NULL; res.as.i = 0;
            if (node->as.unaryop.op == UNARY_MINUS) {
                if (expr.type == VAL_INT) { res.type = VAL_INT; res.as.i = -expr.as.i; }
                else if (expr.type == VAL_FLOAT) { res.type = VAL_FLOAT; res.as.f = -expr.as.f; }
                else { fprintf(stderr, "Invalid operand for unary minus\n"); exit(1); }
            } else if (node->as.unaryop.op == UNARY_NOT) {
                res.type = VAL_BOOL; res.as.b = !is_truthy(expr);
            }
            free_val(expr);
            return res;
        }
        case AST_POSTFIX: {
            Value current = get_var(node->as.postfix.name);
            if (current.type != VAL_INT) { fprintf(stderr, "Postfix ops only on int\n"); exit(1); }
            Value res = current;
            Value new_val; new_val.type = VAL_INT;
            if (node->as.postfix.op == POSTFIX_INC) new_val.as.i = current.as.i + 1;
            else new_val.as.i = current.as.i - 1;
            assign_var(node->as.postfix.name, new_val);
            return res;
        }
        default:
            fprintf(stderr, "Unknown AST node type in evaluate\n");
            exit(1);
    }
}

void interpret(ASTNode* root) {
    srand(time(NULL));
    push_env();
    Value v = evaluate(root);
    free_val(v);
    pop_env();
    
    // Clean function table
    FuncEntry* curr = func_table;
    while (curr) {
        FuncEntry* next = curr->next;
        free(curr->name);
        free(curr);
        curr = next;
    }
}
