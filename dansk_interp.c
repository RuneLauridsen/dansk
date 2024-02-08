static void interp_exit_error(interp *interp, str s) {
    // @Todo: Report source location. println("Parse fejl på linje %, kolonne %", t.linenumber, t.column);
    println("Interp fejl.");
    println("    %", s);
    exit(0);
}

static decl *get_decl_by_ident(interp *interp, str ident) {
    decl *ret = null;

    for_ptr_list(decl, it, interp->decls.first) {
        if (str_eq(it->ident, ident)) {
            ret = it;
            break;
        }
    }

    if (!ret) {
        interp_exit_error(interp, tprint("Variabel % er ikke erklæret.", ident));
    }

    return ret;
}

static var_value cast_var_value(var_value src, var_kind cast_to) {
    var_value ret = { 0 };
    ret.kind = cast_to;

    if (src.kind == cast_to) {
        ret = src;
    } else if (cast_to == VAR_KIND_F64 && src.kind == VAR_KIND_I64) {
        ret.as_f64 = (f64)src.as_i64;
    } else if (cast_to == VAR_KIND_I64 && src.kind == VAR_KIND_F64) {
        ret.as_i64 = (i64)src.as_f64;
    } else {
        assert(!"Invalid code path");
    }

    return ret;
}

static bool cast_var_value_to_bool(var_value a) {
    bool ret = false;
    switch (a.kind) {
        case VAR_KIND_I64: ret = (bool)a.as_i64; break;
        case VAR_KIND_F64: ret = (bool)a.as_f64; break;
        default: assert(!"Invalid code path"); break;
    }
    return ret;
}

static var_value eval_operator(var_value a, var_value b, operator o) {
    b = cast_var_value(b, a.kind); // Make sure a and b are some type. Always prefer type of a.

    var_value ret = { 0 };
    ret.kind = a.kind;

    switch (a.kind) {
        case VAR_KIND_I64: {
            switch (o) {
                case OPERATOR_ADD: ret.as_i64 = (a.as_i64 + b.as_i64); break;
                case OPERATOR_SUB: ret.as_i64 = (a.as_i64 - b.as_i64); break;
                case OPERATOR_MUL: ret.as_i64 = (a.as_i64 * b.as_i64); break;
                case OPERATOR_DIV: ret.as_i64 = (a.as_i64 / b.as_i64); break;
                case OPERATOR_GT:  ret.as_i64 = (a.as_i64 > b.as_i64); break;
                case OPERATOR_LT:  ret.as_i64 = (a.as_i64 < b.as_i64); break;
                case OPERATOR_EQ:  ret.as_i64 = (a.as_i64 == b.as_i64); break;
                default: assert(!"Invalid code path"); break;
            }
        } break;

        case VAR_KIND_F64: {
            switch (o) {
                case OPERATOR_ADD: ret.as_f64 = (a.as_f64 + b.as_f64); break;
                case OPERATOR_SUB: ret.as_f64 = (a.as_f64 - b.as_f64); break;
                case OPERATOR_MUL: ret.as_f64 = (a.as_f64 * b.as_f64); break;
                case OPERATOR_DIV: ret.as_f64 = (a.as_f64 / b.as_f64); break;
                case OPERATOR_GT:  ret.as_f64 = (a.as_f64 > b.as_f64); break;
                case OPERATOR_LT:  ret.as_f64 = (a.as_f64 < b.as_f64); break;
                case OPERATOR_EQ:  ret.as_f64 = (a.as_f64 == b.as_f64); break;
                default: assert(!"Invalid code path"); break;
            }
        } break;

        default: {
            assert(!"Invalid code path");
        } break;
    }

    return ret;
}

static var_value eval_expr(interp *interp, ast_expr *expr) {
    var_value ret = { 0 };
    switch (expr->kind) {
        case AST_EXPR_KIND_IDENT: {
            decl *decl = get_decl_by_ident(interp, expr->as_ident.str);
            ret = decl->value;
        } break;

        case AST_EXPR_KIND_LITERAL: {
            ret = expr->as_literal.value;
        } break;

        case AST_EXPR_KIND_BINOP: {
            var_value left_value  = eval_expr(interp, expr->as_binop.left);
            var_value right_value = eval_expr(interp, expr->as_binop.right);
            ret = eval_operator(left_value, right_value, expr->as_binop.operator);
        } break;
    }

    return ret;
}

static void print_decl(decl *decl) {
    print("% = ", decl->ident);
    print_var_value(decl->value);
}

static void interp_init(interp *interp, ast ast, arena *runtime_storage) {
    zero(interp);
    interp->next_node = ast.root->as_block.nodes.first;
    interp->arena = runtime_storage;
}

static void interp_exec_next_node(interp *interp) {
    ast_node *exec = interp->next_node;
    if (exec) {
        switch (exec->kind) {
            case AST_NODE_KIND_DECL: {
                ast_decl ast = exec->as_decl;

                decl *decl = push_struct(interp->arena, struct decl);
                decl->ident = ast.ident;
                decl->value.kind = ast.var_kind;

                slist_add(&interp->decls, decl);

                interp->next_node = exec->next;
            } break;

            case AST_NODE_KIND_ASSIGN: {
                ast_assign ast = exec->as_assign;
                decl *decl = get_decl_by_ident(interp, ast.l_ident);
                var_value value = eval_expr(interp, ast.r_expr);
                decl->value = cast_var_value(value, decl->value.kind);

                interp->next_node = exec->next;
            } break;

            case AST_NODE_KIND_OUTPUT: {
                ast_output ast = exec->as_output;
                var_value value = eval_expr(interp, ast.expr);
                print_var_value(value);

                interp->next_node = exec->next;
            } break;

            case AST_NODE_KIND_WHILE: {
                ast_while ast = exec->as_while;
                var_value condition = eval_expr(interp, ast.condition);
                if (cast_var_value_to_bool(condition)) {
                    interp->next_node = ast.body;
                } else {
                    interp->next_node = exec->next;
                }
            } break;

            case AST_NODE_KIND_IF: {
                ast_if ast = exec->as_if;
                var_value condition = eval_expr(interp, ast.condition);
                if (cast_var_value_to_bool(condition)) {
                    interp->next_node = ast.body_true;
                } else {
                    interp->next_node = ast.body_false;
                }
            } break;

            case AST_NODE_KIND_BLOCK: {
                ast_block ast = exec->as_block;
                interp->next_node = ast.nodes.first;
            } break;

            default: {
                assert(!"Invalid code path");
            } break;

        }

        // Check wether we reached the end of the current block.
        if (interp->next_node == null) {
            ast_node *next = exec->parent;
            while (next) {
                if (next->kind == AST_NODE_KIND_BLOCK) {
                    next = coalesce(next->next, next->parent);
                } else if (next->kind == AST_NODE_KIND_IF) {
                    next = coalesce(next->next, next->parent);
                    break;
                } else if (next->kind == AST_NODE_KIND_WHILE) {
                    break;
                } else {
                    assert(!"Invalid code path");
                }
            }

            interp->next_node = next;
        }
    }
}
