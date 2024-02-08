
typedef struct ast_decl ast_decl;
struct ast_decl {
    str ident;
    var_kind var_kind;
};

typedef enum ast_expr_kind {
    AST_EXPR_KIND_NONE,
    AST_EXPR_KIND_IDENT,
    AST_EXPR_KIND_LITERAL,
    AST_EXPR_KIND_BINOP,
    AST_EXPR_KIND_COUNT,
} ast_expr_kind;

typedef struct ast_binop ast_binop;
struct ast_binop {
    operator operator;
    struct ast_expr *left;
    struct ast_expr *right;
};

typedef struct ast_ident ast_ident;
struct ast_ident {
    str str;
};

typedef struct ast_literal ast_literal;
struct ast_literal {
    var_value value;
};

typedef struct ast_expr ast_expr;
struct ast_expr {
    ast_expr_kind kind;
    union {
        ast_binop as_binop;
        ast_ident as_ident;
        ast_literal as_literal;
    };
};

typedef struct ast_assign ast_assign;
struct ast_assign {
    str l_ident;
    ast_expr *r_expr;
};

typedef struct ast_output ast_output;
struct ast_output {
    ast_expr *expr;
};

typedef enum ast_node_kind {
    AST_NODE_KIND_NONE,
    AST_NODE_KIND_DECL,
    AST_NODE_KIND_ASSIGN,
    AST_NODE_KIND_OUTPUT,
    AST_NODE_KIND_IF,
    AST_NODE_KIND_WHILE,
    AST_NODE_KIND_BLOCK,
    AST_NODE_KIND_COUNT,
} ast_node_kind;

typedef struct ast_node ast_node;

typedef struct ast_block ast_block;
struct ast_block {
    list(ast_node) nodes;
};

typedef struct ast_if ast_if;
struct ast_if {
    ast_expr *condition;
    ast_node *body_true;
    ast_node *body_false;
};

typedef struct ast_while ast_while;
struct ast_while {
    ast_expr *condition;
    ast_node *body;
};

typedef struct ast_node ast_node;
struct ast_node {
    ast_node_kind kind;
    union {
        ast_decl as_decl;
        ast_assign as_assign;
        ast_output as_output;
        ast_if as_if;
        ast_while as_while;
        ast_block as_block;
    };
    ast_node *next;
    ast_node *parent;
};

typedef struct ast ast;
struct ast {
    ast_node *root;
};

static void print_ast(ast ast);

////////////////////////////////////////////////////////////////
// Enum name tables.

static readonly str ast_node_kind_names[] = {
   [AST_NODE_KIND_NONE] = static_str("AST_NODE_KIND_NONE"),
   [AST_NODE_KIND_DECL] = static_str("AST_NODE_KIND_DECL"),
   [AST_NODE_KIND_ASSIGN] = static_str("AST_NODE_KIND_ASSIGN"),
   [AST_NODE_KIND_OUTPUT] = static_str("AST_NODE_KIND_OUTPUT"),
   [AST_NODE_KIND_COUNT] = static_str("AST_NODE_KIND_COUNT"),
};

static readonly str ast_expr_kind_names[] = {
    [AST_EXPR_KIND_NONE] = static_str("AST_EXPR_KIND_NONE"),
    [AST_EXPR_KIND_IDENT] = static_str("AST_EXPR_KIND_IDENT"),
    [AST_EXPR_KIND_LITERAL] = static_str("AST_EXPR_KIND_LITERAL"),
    [AST_EXPR_KIND_BINOP] = static_str("AST_EXPR_KIND_BINOP"),
    [AST_EXPR_KIND_COUNT] = static_str("AST_EXPR_KIND_COUNT"),
};

static readonly str var_kind_names[] = {
    [VAR_KIND_NONE] = static_str("VAR_KIND_NONE"),
    [VAR_KIND_I64] = static_str("VAR_KIND_I64"),
    [VAR_KIND_F64] = static_str("VAR_KIND_F64"),
    [VAR_KIND_COUNT] = static_str("VAR_KIND_COUNT"),
};

////////////////////////////////////////////////////////////////
// String converion functions.

static str ast_node_kind_as_str(ast_node_kind a);
static str ast_expr_kind_as_str(ast_expr_kind a);
static str var_kind_as_str(var_kind a);
