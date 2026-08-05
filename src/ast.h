#ifndef AST_H
#define AST_H

/**
 * @brief Enum representing the various types of nodes in the Abstract Syntax Tree (AST).
 */
typedef enum {
    AST_PROGRAM,           /**< Root node of a program containing multiple statements */
    AST_BLOCK,             /**< A block of statements (e.g., inside `{ }`) */
    AST_SHOW,              /**< A built-in show/print statement */
    AST_LET,               /**< Variable declaration (e.g., `let x = 5`) */
    AST_ASSIGN,            /**< Variable assignment (e.g., `x = 10`) */
    AST_COMPOUND_ASSIGN,   /**< Compound assignment (e.g., `x += 5`) */
    AST_IF,                /**< If-else conditional statement */
    AST_WHILE,             /**< While loop statement */
    AST_FOR,               /**< For loop statement */
    AST_REPEAT,            /**< Repeat loop statement */
    AST_FUNC_DECL,         /**< Function declaration */
    AST_RETURN,            /**< Return statement */
    AST_CALL,              /**< Function call expression */
    AST_INT_LITERAL,       /**< Integer literal expression */
    AST_FLOAT_LITERAL,     /**< Float literal expression */
    AST_STRING_LITERAL,    /**< String literal expression */
    AST_BOOL_LITERAL,      /**< Boolean literal expression */
    AST_IDENTIFIER,        /**< Variable identifier expression */
    AST_BINOP,             /**< Binary operation expression */
    AST_UNARYOP,           /**< Unary operation expression */
    AST_POSTFIX            /**< Postfix operation expression (e.g., `x++`) */
} ASTNodeType;

/**
 * @brief Enum for binary operations.
 */
typedef enum {
    BINOP_ADD, BINOP_SUB, BINOP_MUL, BINOP_DIV, BINOP_MOD,
    BINOP_GT, BINOP_LT, BINOP_GTE, BINOP_LTE, BINOP_EQ, BINOP_NEQ,
    BINOP_AND, BINOP_OR
} BinOpType;

/**
 * @brief Enum for unary operations.
 */
typedef enum {
    UNARY_MINUS, UNARY_NOT
} UnaryOpType;

/**
 * @brief Enum for postfix operations.
 */
typedef enum {
    POSTFIX_INC, POSTFIX_DEC
} PostfixOpType;

/**
 * @brief Structure representing a node in the Abstract Syntax Tree.
 */
typedef struct ASTNode {
    ASTNodeType type;      /**< The type of the AST node */
    union {
        struct {
            struct ASTNode** stmts; /**< Array of statements */
            int count;              /**< Number of statements */
            int capacity;           /**< Allocated capacity of the statements array */
        } program;
        struct {
            struct ASTNode** stmts; /**< Array of statements in the block */
            int count;              /**< Number of statements */
            int capacity;           /**< Allocated capacity of the statements array */
        } block;
        struct {
            struct ASTNode* expr;   /**< Expression to evaluate and show */
        } show;
        struct {
            char* name;             /**< Name of the declared variable */
            struct ASTNode* expr;   /**< Initial value expression */
        } let_decl;
        struct {
            char* name;             /**< Name of the assigned variable */
            struct ASTNode* expr;   /**< Assigned value expression */
        } assign;
        struct {
            BinOpType op;           /**< Compound operator */
            char* name;             /**< Name of the assigned variable */
            struct ASTNode* expr;   /**< Value expression */
        } compound_assign;
        struct {
            struct ASTNode* cond;        /**< Condition expression */
            struct ASTNode* then_branch; /**< Branch to execute if true */
            struct ASTNode* else_branch; /**< Branch to execute if false (can be NULL) */
        } if_stmt;
        struct {
            struct ASTNode* cond;        /**< Loop condition */
            struct ASTNode* body;        /**< Loop body */
        } while_stmt;
        struct {
            struct ASTNode* init;        /**< Initialization statement */
            struct ASTNode* cond;        /**< Loop condition expression */
            struct ASTNode* inc;         /**< Increment expression */
            struct ASTNode* body;        /**< Loop body */
        } for_stmt;
        struct {
            struct ASTNode* count_expr;  /**< Number of times to repeat */
            struct ASTNode* body;        /**< Loop body */
        } repeat_stmt;
        struct {
            char* name;                  /**< Function name */
            char** params;               /**< Array of parameter names */
            int param_count;             /**< Number of parameters */
            int param_capacity;          /**< Allocated capacity for parameters */
            struct ASTNode* body;        /**< Function body block */
        } func_decl;
        struct {
            struct ASTNode* expr;        /**< Expression to return */
        } return_stmt;
        struct {
            char* name;                  /**< Function name to call */
            struct ASTNode** args;       /**< Array of argument expressions */
            int arg_count;               /**< Number of arguments */
            int arg_capacity;            /**< Allocated capacity for arguments */
        } call_expr;
        struct {
            long long value;             /**< Integer value */
        } int_literal;
        struct {
            double value;                /**< Float value */
        } float_literal;
        struct {
            char* value;                 /**< String value */
        } string_literal;
        struct {
            int value;                   /**< Boolean value (0 or 1) */
        } bool_literal;
        struct {
            char* name;                  /**< Variable name */
        } identifier;
        struct {
            BinOpType op;                /**< Binary operator type */
            struct ASTNode* left;        /**< Left operand */
            struct ASTNode* right;       /**< Right operand */
        } binop;
        struct {
            UnaryOpType op;              /**< Unary operator type */
            struct ASTNode* expr;        /**< Operand expression */
        } unaryop;
        struct {
            PostfixOpType op;            /**< Postfix operator type */
            char* name;                  /**< Variable name */
        } postfix;
    } as; /**< Union containing specific fields for each AST node type */
} ASTNode;

/**
 * @brief Creates a new program node.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_program();

/**
 * @brief Adds a statement to a program node.
 * @param prog The program node.
 * @param stmt The statement node to add.
 */
void program_add_stmt(ASTNode* prog, ASTNode* stmt);

/**
 * @brief Creates a new block node.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_block();

/**
 * @brief Adds a statement to a block node.
 * @param block The block node.
 * @param stmt The statement node to add.
 */
void block_add_stmt(ASTNode* block, ASTNode* stmt);

/**
 * @brief Creates a new show statement node.
 * @param expr The expression to display.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_show(ASTNode* expr);

/**
 * @brief Creates a new variable declaration node.
 * @param name The name of the variable.
 * @param expr The initialization expression.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_let(const char* name, ASTNode* expr);

/**
 * @brief Creates a new assignment statement node.
 * @param name The name of the variable to assign.
 * @param expr The value expression.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_assign(const char* name, ASTNode* expr);

/**
 * @brief Creates a new compound assignment node (e.g., +=, -=).
 * @param op The binary operation type.
 * @param name The name of the variable.
 * @param expr The value expression.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_compound_assign(BinOpType op, const char* name, ASTNode* expr);

/**
 * @brief Creates a new if-else conditional node.
 * @param cond The condition expression.
 * @param then_branch The block or statement to execute if true.
 * @param else_branch The block or statement to execute if false (can be NULL).
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_if(ASTNode* cond, ASTNode* then_branch, ASTNode* else_branch);

/**
 * @brief Creates a new while loop node.
 * @param cond The loop condition expression.
 * @param body The loop body block or statement.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_while(ASTNode* cond, ASTNode* body);

/**
 * @brief Creates a new for loop node.
 * @param init The initialization statement.
 * @param cond The condition expression.
 * @param inc The increment expression.
 * @param body The loop body block or statement.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_for(ASTNode* init, ASTNode* cond, ASTNode* inc, ASTNode* body);

/**
 * @brief Creates a new repeat loop node.
 * @param count_expr The expression specifying the number of iterations.
 * @param body The loop body block or statement.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_repeat(ASTNode* count_expr, ASTNode* body);

/**
 * @brief Creates a new function declaration node.
 * @param name The name of the function.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_func_decl(const char* name);

/**
 * @brief Adds a parameter to a function declaration.
 * @param func The function declaration node.
 * @param param The name of the parameter.
 */
void func_decl_add_param(ASTNode* func, const char* param);

/**
 * @brief Creates a new return statement node.
 * @param expr The expression to return.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_return(ASTNode* expr);

/**
 * @brief Creates a new function call node.
 * @param name The name of the function to call.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_call(const char* name);

/**
 * @brief Adds an argument to a function call node.
 * @param call The function call node.
 * @param arg The argument expression to add.
 */
void call_add_arg(ASTNode* call, ASTNode* arg);

/**
 * @brief Creates a new integer literal node.
 * @param val The integer value.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_int_literal(long long val);

/**
 * @brief Creates a new floating-point literal node.
 * @param val The float value.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_float_literal(double val);

/**
 * @brief Creates a new string literal node.
 * @param val The string value (will be copied).
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_string_literal(const char* val);

/**
 * @brief Creates a new boolean literal node.
 * @param val The boolean value (0 for false, non-zero for true).
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_bool_literal(int val);

/**
 * @brief Creates a new identifier (variable reference) node.
 * @param name The variable name.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_identifier(const char* name);

/**
 * @brief Creates a new binary operation node.
 * @param op The binary operator type.
 * @param left The left operand expression.
 * @param right The right operand expression.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_binop(BinOpType op, ASTNode* left, ASTNode* right);

/**
 * @brief Creates a new unary operation node.
 * @param op The unary operator type.
 * @param expr The operand expression.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_unaryop(UnaryOpType op, ASTNode* expr);

/**
 * @brief Creates a new postfix operation node (e.g., post-increment).
 * @param op The postfix operator type.
 * @param name The variable name.
 * @return A pointer to the newly allocated ASTNode.
 */
ASTNode* make_postfix(PostfixOpType op, const char* name);

/**
 * @brief Recursively frees all memory associated with an AST node and its children.
 * @param node The AST node to free.
 */
void free_ast(ASTNode* node);

#endif // AST_H
