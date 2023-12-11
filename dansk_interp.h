typedef struct decl decl;
struct decl {
    str ident;
    var_value value;

    decl *next; // @Size
};

typedef struct decls decls;
struct decls {
    decl *first;
    decl *last;
};

typedef struct interp interp;
struct interp {
    ast ast;
    decls decls;
    ast_node *next_node;
    arena *arena;
};

static void interp_init(interp *interp, ast ast, arena *runtime_storage);
static void interp_exec_next_node(interp *interp);
