static str ast_node_kind_as_str(ast_node_kind a) {
    assert(a < countof(ast_node_kind_names));
    str ret = ast_node_kind_names[a];
    return ret;
}

static str ast_expr_kind_as_str(ast_expr_kind a) {
    assert(a < countof(ast_expr_kind_names));
    str ret = ast_expr_kind_names[a];
    return ret;
}

static str var_kind_as_str(var_kind a) {
    assert(a < countof(var_kind_names));
    str ret = var_kind_names[a];
    return ret;
}

static void print_indentation(i32 depth) {
    while (depth--) {
        print("    ");
    }
}

static void print_ast_expr(ast_expr *expr, i32 depth) {
    switch (expr->kind) {
        case AST_EXPR_KIND_IDENT: {
            print_indentation(depth);
            println("expr-ident %var", expr->as_ident.str);
        } break;

        case AST_EXPR_KIND_LITERAL: {
            print_indentation(depth);
            println("expr-literal %", expr->as_literal.i64);
        } break;

        case AST_EXPR_KIND_BINOP: {
            print_indentation(depth);
            println("expr-binop %", operator_as_str(expr->as_binop.operator));

            print_indentation(depth + 1);
            println("left");
            print_ast_expr(expr->as_binop.left, depth + 2);

            print_indentation(depth + 1);
            println("right");
            print_ast_expr(expr->as_binop.right, depth + 2);
        } break;

        default: {
            assert(!"Invalid code path.");
        } break;
    }
}

static void print_ast_node(ast_node *node, i32 depth) {
    switch (node->kind) {
        case AST_NODE_KIND_DECL: {
            print_indentation(depth);
            ast_decl decl = node->as_decl;
            println("node-decl %var %", decl.ident, var_kind_as_str(decl.var_kind));
        } break;

        case AST_NODE_KIND_ASSIGN: {
            print_indentation(depth);
            ast_assign assign = node->as_assign;
            println("node-assign %var", assign.l_ident);
            print_ast_expr(assign.r_expr, depth + 1);
        } break;

        case AST_NODE_KIND_OUTPUT: {
            print_indentation(depth);
            ast_output output = node->as_output;
            println("node-output");
            print_ast_expr(output.expr, depth + 1);
        } break;

        case AST_NODE_KIND_IF: {
            print_indentation(depth);
            ast_if if_ = node->as_if;
            println("node-if");

            print_indentation(depth + 1);
            println("condition");
            print_ast_expr(if_.condition, depth + 2);

            print_indentation(depth + 1);
            println("body-true");
            print_ast_node(if_.body_true, depth + 2);

            print_indentation(depth + 1);
            println("body-false");
            print_ast_node(if_.body_false, depth + 2);
        } break;

        case AST_NODE_KIND_WHILE: {
            print_indentation(depth);
            ast_while while_ = node->as_while;
            println("node-while");

            print_indentation(depth + 1);
            println("condition");
            print_ast_expr(while_.condition, depth + 2);

            print_indentation(depth + 1);
            println("body");
            print_ast_node(while_.body, depth + 2);
        } break;

        case AST_NODE_KIND_BLOCK: {
            print_indentation(depth);
            ast_block body = node->as_block;
            println("node-block");
            for_ptr_list(ast_node, it, body.nodes.first) {
                print_ast_node(it, depth + 1);
            }
        } break;

        default: {
            assert(!"Invalid code path.");
        } break;
    }
}

static void print_ast(ast ast) {
    print_ast_node(ast.root, 0);
}
