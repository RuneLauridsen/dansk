////////////////////////////////////////////////////////////////
//
//
// Errors
//
//
////////////////////////////////////////////////////////////////

#define dk_tprint(...) dk_tprint_args(argsof(__VA_ARGS__))
static str dk_tprint_args(args args) {
    str ret = arena_push_fmt_args(&temp_arena, args);
    return ret;
}

#if 0
static _Noreturn void dk_exit_err(str msg) {
    println(msg);
    debug_break();
    exit(1);
}
#endif

static dk_err *dk_err_list_add(dk_err_list *list, arena *arena) {
    dk_err *node = arena_push_struct(arena, dk_err);
    slist_add(list, node);
    list->count += 1;
    return node;
}

static void dk_report_err(dk_err_sink *sink, str msg) {
    dk_err *node = dk_err_list_add(&sink->err_list, &temp_arena);
    node->msg = msg;
}

static str dk_str_from_err_sink(dk_err_sink *sink, arena *arena) {
    str_list list = { 0 };
    for_list (dk_err, err, sink->err_list) {
        str_list_push_fmt(&list, arena, "%\n", err->msg);
    }

    str ret = str_list_join(&list, arena, str(""));
    return ret;
}

static dk_err_sink *dk_global_err = null; // TODO(rune): Remove global.

////////////////////////////////////////////////////////////////
//
//
// Tokenizer
//
//
////////////////////////////////////////////////////////////////

static void dk_tokenizer_eat(dk_tokenizer *t) {
    t->pos += 1;
    t->col += 1;

    if (t->peek0 == '\n') {
        t->col = 0;
        t->row += 1;
    }

    t->peek0 = (t->pos + 0 < t->src.len) ? (t->src.v[t->pos + 0]) : ('\0');
    t->peek1 = (t->pos + 1 < t->src.len) ? (t->src.v[t->pos + 1]) : ('\0');
}

static void dk_tokenizer_init(dk_tokenizer *t, str src, u64 pos) {
    t->src = src;
    t->pos = pos - 1;
    dk_tokenizer_eat(t);
}

static str dk_token_kind_as_str(dk_token_kind a) {
    switch (a) {
        case DK_TOKEN_KIND_EOF:             return str("DK_TOKEN_KIND_EOF");

        case DK_TOKEN_KIND_ASSIGN:          return str("DK_TOKEN_KIND_ASSIGN");
        case DK_TOKEN_KIND_ADD:             return str("DK_TOKEN_KIND_ADD");
        case DK_TOKEN_KIND_SUB:             return str("DK_TOKEN_KIND_SUB");
        case DK_TOKEN_KIND_MUL:             return str("DK_TOKEN_KIND_MUL");
        case DK_TOKEN_KIND_DIV:             return str("DK_TOKEN_KIND_DIV");
        case DK_TOKEN_KIND_AND:             return str("DK_TOKEN_KIND_AND");
        case DK_TOKEN_KIND_OR:              return str("DK_TOKEN_KIND_OR");
        case DK_TOKEN_KIND_LESS:            return str("DK_TOKEN_KIND_LESS");
        case DK_TOKEN_KIND_GREATER:         return str("DK_TOKEN_KIND_GREATER");

        case DK_TOKEN_KIND_DOT:             return str("DK_TOKEN_KIND_DOT");
        case DK_TOKEN_KIND_COMMA:           return str("DK_TOKEN_KIND_COMMA");
        case DK_TOKEN_KIND_PAREN_OPEN:      return str("DK_TOKEN_KIND_PAREN_OPEN");
        case DK_TOKEN_KIND_PAREN_CLOSE:     return str("DK_TOKEN_KIND_PAREN_CLOSE");

        case DK_TOKEN_KIND_IDENT:           return str("DK_TOKEN_KIND_IDENT");
        case DK_TOKEN_KIND_LITERAL_INT:     return str("DK_TOKEN_KIND_LITERAL_INT");
        case DK_TOKEN_KIND_LITERAL_FLOAT:   return str("DK_TOKEN_KIND_LITERAL_FLOAT");

        case DK_TOKEN_KIND_PUBLIC:          return str("DK_TOKEN_KIND_PUBLIC");
        case DK_TOKEN_KIND_PROC:            return str("DK_TOKEN_KIND_PROC");
        case DK_TOKEN_KIND_BEGIN:           return str("DK_TOKEN_KIND_BEGIN");
        case DK_TOKEN_KIND_END:             return str("DK_TOKEN_KIND_END");
        case DK_TOKEN_KIND_LET:             return str("DK_TOKEN_KIND_LET");
        case DK_TOKEN_KIND_BE:              return str("DK_TOKEN_KIND_BE");
        case DK_TOKEN_KIND_TO:              return str("DK_TOKEN_KIND_TO");
        case DK_TOKEN_KIND_AS:              return str("DK_TOKEN_KIND_AS");
        case DK_TOKEN_KIND_CALL:            return str("DK_TOKEN_KIND_CALL");
        case DK_TOKEN_KIND_EN_ET:           return str("DK_TOKEN_KIND_EN_ET");
        case DK_TOKEN_KIND_RETURN:          return str("DK_TOKEN_KIND_RETURN");
        case DK_TOKEN_KIND_COMMENT:         return str("DK_TOKEN_KIND_COMMENT");
        case DK_TOKEN_KIND_IF:              return str("DK_TOKEN_KIND_IF");
        case DK_TOKEN_KIND_ELSE:            return str("DK_TOKEN_KIND_ELSE");
        case DK_TOKEN_KIND_WHILE:           return str("DK_TOKEN_KIND_WHILE");
        case DK_TOKEN_KIND_ARGUMENT:        return str("DK_TOKEN_KIND_ARGUMENT");

        case DK_TOKEN_KIND_FALSE:           return str("DK_TOKEN_KIND_FALSE");
        case DK_TOKEN_KIND_TRUE:            return str("DK_TOKEN_KIND_TRUE");

        default:                            return str("INVALID");
    }
}

static dk_token_kind dk_token_kind_from_spelling(str s) {
    dk_token_kind token_kind = 0;

    for_array (dk_token_spelling, it, dk_token_spellings) {
        if (str_eq_nocase(it->spelling, s)) {
            token_kind = it->token_kind;
            break;
        }
    }

    return token_kind;
}

static bool dk_next_token(dk_tokenizer *t, dk_token *token) {
    zero(token);

    //- Eat whitespace
    while (u8_get_char_flags(t->peek0) & CHAR_FLAG_WHITESPACE) {
        dk_tokenizer_eat(t);
    }

    dk_token_kind kind = 0;
    u64 begin_pos = t->pos;
    u64 begin_row = t->row;
    u64 begin_col = t->col;

    //- Punctuation token
    if (u8_is_punct(t->peek0)) {
        dk_tokenizer_eat(t); // TODO(rune): Multi-character operators like +=

        str s = substr_len(t->src, begin_pos, t->pos - begin_pos);
        kind = dk_token_kind_from_spelling(s);
        if (kind == 0) {
            dk_report_err(dk_global_err, dk_tprint("Invalid operator at %:%", t->row + 1, t->col + 1));
        }
    }

    //- Word/keyword token
    else if (u8_is_letter(t->peek0) || t->peek0 == '_') {
        while (u8_is_letter(t->peek0) || u8_is_digit(t->peek0) || t->peek0 == '_' || t->peek0 >= 128) {
            dk_tokenizer_eat(t);
        }

        str s = substr_len(t->src, begin_pos, t->pos - begin_pos);
        kind = dk_token_kind_from_spelling(s);
        if (kind == 0) {
            kind = DK_TOKEN_KIND_IDENT;
        }

        //- Continue comment until newline.
        if (kind == DK_TOKEN_KIND_COMMENT) {
            while (t->peek0 != '\n') {
                dk_tokenizer_eat(t);
            }
        }
    }

    //- Number token
    else if (u8_is_digit(t->peek0)) {
        bool got_dec_sep = false;

        while (u8_is_digit(t->peek0) || t->peek0 == ',') {
            if (t->peek0 == ',') {
                got_dec_sep = true;
            }
            dk_tokenizer_eat(t);
        }

        str s = substr_len(t->src, begin_pos, t->pos - begin_pos);
        if (got_dec_sep) {

            //- Parse float.
            bool fmt_ok = false;
            char temp[256];
            if (s.len + 1 < sizeof(temp)) {
                memcpy(temp, s.v, s.len);
                temp[s.len] = '\0';
                for_n (u64, i, s.len) {
                    if (temp[i] == ',') temp[i] = '.';
                }

                char *end_ptr = temp;
                token->f64_ = strtod(temp, &end_ptr);

                if (end_ptr == temp + s.len) {
                    fmt_ok = true;
                }
            }

            if (fmt_ok) {
                kind = DK_TOKEN_KIND_LITERAL_FLOAT;
            } else {
                dk_report_err(dk_global_err, str("Invalid floating point literal.")); // TODO(rune): Better error message.
            }
        } else {
            //- Parse integer.
            // TODO(rune): Check for overflow
            kind = DK_TOKEN_KIND_LITERAL_INT;
            for_n (u64, i, s.len) {
                u8 c = s.v[i];
                token->i64_ *= 10;
                token->i64_ += c - '0';
            }
        }
    }

    u64 end_pos   = t->pos;
    token->kind   = kind;
    token->text   = substr_len(t->src, begin_pos, end_pos - begin_pos);
    token->pos    = begin_pos;
    token->row    = begin_row;
    token->col    = begin_col;

    return kind != 0;
}

////////////////////////////////////////////////////////////////
//
//
// Parser
//
//
////////////////////////////////////////////////////////////////

static dk_token dk_eat_token(dk_parser *p) {
    dk_token consumed = p->peek;
    dk_next_token(&p->tokenizer, &p->peek);

    // Skip comments.
    while (p->peek.kind == DK_TOKEN_KIND_COMMENT) {
        dk_next_token(&p->tokenizer, &p->peek);
    }

#if DK_DEBUG_PRINT_LEX
    dk_print_token(&p->peek);
#endif
    return consumed;
}

static dk_token dk_eat_token_kind(dk_parser *p, dk_token_kind kind) {
    if (dk_global_err->err_list.count > 0) return (dk_token) { 0 };

    dk_token consumed = dk_eat_token(p);
    if (consumed.kind != kind) {
        dk_report_err(dk_global_err, dk_tprint("Unexpected token at %:%", consumed.row + 1, consumed.col + 1)); // TODO(rune): Better error message.
    }
    return consumed;
}

static bool dk_eat_token_kind_maybe(dk_parser *p, dk_token_kind kind) {
    bool was_consumed = false;
    if (p->peek.kind == kind) {
        dk_eat_token_kind(p, kind);
        was_consumed = true;
    }
    return was_consumed;
}

static void dk_parser_init(dk_parser *p, str s) {
    dk_tokenizer_init(&p->tokenizer, s, 0);
    dk_next_token(&p->tokenizer, &p->peek);
}

static dk_ast_literal dk_parse_literal(dk_parser *p) {
    // TODO(rune): More literal types.
    // TODO(rune): This seems dumb. Do we really need the dk_ast_literal_kind enum?

    dk_ast_literal literal = { 0 };

    dk_token t = dk_eat_token(p);
    switch (t.kind) {
        case DK_TOKEN_KIND_LITERAL_INT: {
            literal.kind = DK_AST_LITERAL_KIND_INT;
            literal.i64_ = t.i64_;
        } break;

        case DK_TOKEN_KIND_LITERAL_FLOAT: {
            literal.kind = DK_AST_LITERAL_KIND_FLOAT;
            literal.i64_ = t.i64_;
        } break;

        case DK_TOKEN_KIND_FALSE: {
            literal.kind = DK_AST_LITERAL_KIND_BOOL;
            literal.boolean = false;
        } break;

        case DK_TOKEN_KIND_TRUE: {
            literal.kind = DK_AST_LITERAL_KIND_BOOL;
            literal.boolean = true;
        } break;
    }

    return literal;
}

// TODO(rune): Table-ify
static dk_ast_expr *dk_parse_expr(dk_parser *p, u64 precedence) {
    dk_ast_expr *expr = arena_push_struct(&dk_ast_arena, dk_ast_expr);;

    //- Function call "Kald Print A plus 1"
    if (dk_eat_token_kind_maybe(p, DK_TOKEN_KIND_CALL)) {
        precedence = dk_operator_infos[DK_TOKEN_KIND_CALL].precedence;

        expr->kind = DK_AST_EXPR_KIND_CALL;
        expr->call.ident = dk_eat_token_kind(p, DK_TOKEN_KIND_IDENT).text;

        while (1) {
            u64 peek_precedence = dk_operator_infos_prefix[p->peek.kind].precedence;
            if (peek_precedence > precedence || peek_precedence == 0) {
                break;
            }

            dk_ast_expr_arg *arg = arena_push_struct(&dk_ast_arena, dk_ast_expr_arg);
            arg->expr = dk_parse_expr(p, 0);
            slist_add(&expr->call.args, arg);
            expr->call.args.count += 1;

            if (!dk_eat_token_kind_maybe(p, DK_TOKEN_KIND_COMMA)) {
                break;
            }
        }
    }

    else {
        if (dk_eat_token_kind_maybe(p, DK_TOKEN_KIND_PAREN_OPEN)) {
            expr = dk_parse_expr(p, 0);
            dk_eat_token_kind(p, DK_TOKEN_KIND_PAREN_CLOSE);
        } else {
            dk_operator_info prefix_info = dk_operator_infos_prefix[p->peek.kind];

            switch (prefix_info.expr_kind) {
                case DK_AST_EXPR_KIND_NONE: {
                    // NOTE(rune): Not a prefix operator, just ignore.
                } break;

                case DK_AST_EXPR_KIND_IDENT: {
                    expr->kind  = DK_AST_EXPR_KIND_IDENT;
                    expr->ident = dk_eat_token_kind(p, DK_TOKEN_KIND_IDENT).text;
                } break;

                case DK_AST_EXPR_KIND_LITERAL: {
                    expr->kind    = DK_AST_EXPR_KIND_LITERAL;;
                    expr->literal = dk_parse_literal(p);;
                } break;

                default: {
                    dk_report_err(dk_global_err, str("Invalid syntax.")); // TODO(rune): Better error message.
                } break;
            }
        }

        while (1) {
            dk_operator_info *infix_operator = &dk_operator_infos[p->peek.kind];
            if (precedence >= infix_operator->precedence) {
                break;
            }

            dk_ast_expr *infix = arena_push_struct(&dk_ast_arena, dk_ast_expr);
            switch (infix_operator->expr_kind) {
                case DK_AST_EXPR_KIND_BINARY: {
                    infix->kind            = DK_AST_EXPR_KIND_BINARY;
                    infix->binary.lhs     = expr;
                    infix->binary.operator = dk_eat_token(p).kind;
                    infix->binary.rhs    = dk_parse_expr(p, infix_operator->precedence);
                } break;

                default: {
                    assert(false && "Invalid operator kind.");
                } break;
            }

            expr = infix;
        }
    }

    return expr;
}

static dk_ast_stmt *dk_parse_stmt(dk_parser *p) {
    dk_ast_stmt *stmt = arena_push_struct(&dk_ast_arena, dk_ast_stmt);

    //- Declaration statement.
    if (p->peek.kind == DK_TOKEN_KIND_LET) {
        dk_eat_token_kind(p, DK_TOKEN_KIND_LET);

        dk_token ident_token = dk_eat_token_kind(p, DK_TOKEN_KIND_IDENT);
        dk_eat_token_kind(p, DK_TOKEN_KIND_BE);
        dk_eat_token_kind_maybe(p, DK_TOKEN_KIND_EN_ET);
        dk_token type_token = dk_eat_token_kind(p, DK_TOKEN_KIND_IDENT);

        //- Optional initial value expression.
        dk_ast_expr *expr = null;
        if (p->peek.kind != DK_TOKEN_KIND_DOT) {
            expr = dk_parse_expr(p, 0);
        }

        dk_eat_token_kind(p, DK_TOKEN_KIND_DOT);

        stmt->kind            = DK_AST_STMT_KIND_DECL;
        stmt->decl.ident      = ident_token.text;
        stmt->decl.type_name  = type_token.text;
        stmt->decl.expr       = expr;
    }

    //- Assignment statement. "Sæt A til A plus 1"
    else if (dk_eat_token_kind_maybe(p, DK_TOKEN_KIND_ASSIGN)) {
        stmt->kind = DK_AST_STMT_KIND_ASSIGN;
        stmt->assign.left = dk_parse_expr(p, 0);
        dk_eat_token_kind(p, DK_TOKEN_KIND_TO);
        stmt->assign.right = dk_parse_expr(p, 0);
        dk_eat_token_kind(p, DK_TOKEN_KIND_DOT);
    }

    //- Return statement.
    else if (dk_eat_token_kind_maybe(p, DK_TOKEN_KIND_RETURN)) {
        stmt->kind = DK_AST_STMT_KIND_RETURN;

        if (p->peek.kind != DK_TOKEN_KIND_DOT) {
            stmt->return_.expr = dk_parse_expr(p, 0);
        }

        dk_eat_token_kind(p, DK_TOKEN_KIND_DOT);
    }

    //- If statement.
    else if (dk_eat_token_kind_maybe(p, DK_TOKEN_KIND_IF)) {
        stmt->kind = DK_AST_STMT_KIND_IF;
        stmt->if_.expr = dk_parse_expr(p, 0);
        dk_eat_token_kind(p, DK_TOKEN_KIND_DOT);
        stmt->if_.then = dk_parse_stmts(p);

        if (dk_eat_token_kind_maybe(p, DK_TOKEN_KIND_ELSE)) {
            dk_eat_token_kind(p, DK_TOKEN_KIND_DOT);
            stmt->if_.else_ = dk_parse_stmts(p);
        }
    }

    //- While statement.
    else if (dk_eat_token_kind_maybe(p, DK_TOKEN_KIND_WHILE)) {
        stmt->kind = DK_AST_STMT_KIND_WHILE;
        stmt->while_.expr = dk_parse_expr(p, 0);
        dk_eat_token_kind(p, DK_TOKEN_KIND_DOT);
        stmt->while_.body = dk_parse_stmts(p);

    }

    //- Expression statement.
    else {
        dk_ast_expr *expr = dk_parse_expr(p, 0);
        dk_eat_token_kind(p, DK_TOKEN_KIND_DOT);

        stmt->kind = DK_AST_STMT_KIND_EXPR;
        stmt->expr = expr;
    }

    return stmt;
}

static dk_ast_stmts dk_parse_stmts(dk_parser *p) {
    dk_ast_stmts stmts = { 0 };

    //- Multi statement block.
    if (p->peek.kind == DK_TOKEN_KIND_BEGIN) {
        dk_eat_token_kind(p, DK_TOKEN_KIND_BEGIN);
        dk_eat_token_kind(p, DK_TOKEN_KIND_DOT);
        while (p->peek.kind != DK_TOKEN_KIND_END && dk_global_err->err_list.count == 0) {
            dk_ast_stmt *stmt = dk_parse_stmt(p);
            slist_add(&stmts, stmt);
        }
        dk_eat_token_kind(p, DK_TOKEN_KIND_END);
        dk_eat_token_kind(p, DK_TOKEN_KIND_DOT);
    }

    //- Single statement block.
    else {
        dk_ast_stmt *stmt = dk_parse_stmt(p);
        slist_add(&stmts, stmt);
    }

    return stmts;
}

static dk_ast_proc dk_parse_proc(dk_parser *p) {
    dk_ast_proc proc = { 0 };

    //- Name.
    dk_eat_token_kind(p, DK_TOKEN_KIND_PROC);
    dk_token ident_token = dk_eat_token_kind(p, DK_TOKEN_KIND_IDENT);
    proc.ident = ident_token.text;

    //- Arguments.
    while (p->peek.kind == DK_TOKEN_KIND_ARGUMENT) {
        // NOTE(rune): "tager A som heltal"
        dk_eat_token_kind(p, DK_TOKEN_KIND_ARGUMENT);
        dk_token arg_name_token = dk_eat_token_kind(p, DK_TOKEN_KIND_IDENT);
        dk_eat_token_kind(p, DK_TOKEN_KIND_AS);
        dk_token arg_type_token = dk_eat_token_kind(p, DK_TOKEN_KIND_IDENT);
        dk_eat_token_kind(p, DK_TOKEN_KIND_COMMA);

        dk_ast_proc_arg *arg = arena_push_struct(&dk_ast_arena, dk_ast_proc_arg);
        arg->name      = arg_name_token.text;
        arg->type_name = arg_type_token.text;

        slist_add(&proc.args, arg);
        proc.args.count++;
    }

    //- Return type.
    dk_eat_token_kind(p, DK_TOKEN_KIND_RETURN);
    dk_eat_token_kind_maybe(p, DK_TOKEN_KIND_EN_ET);
    dk_token type_token = dk_eat_token_kind(p, DK_TOKEN_KIND_IDENT);
    proc.return_type_name = type_token.text;

    // TODO(rune): Function arguments.
    dk_eat_token_kind(p, DK_TOKEN_KIND_DOT);

    //- Body
    proc.stmts = dk_parse_stmts(p);


    return proc;
}

static dk_ast_visibilty dk_parse_visibility(dk_parser *p) {
    dk_ast_visibilty visibilty = 0;

    if (dk_eat_token_kind_maybe(p, DK_TOKEN_KIND_PUBLIC)) {
        visibilty = DK_AST_VISIBILTY_PUBLIC;
    } else {
        visibilty = DK_AST_VISIBILTY_PRIVATE;
    }

    return visibilty;
}

static dk_ast_symbol *dk_parse_symbol(dk_parser *p) {
    dk_ast_symbol *symbol = arena_push_struct(&dk_ast_arena, dk_ast_symbol);

    dk_ast_visibilty visbility = dk_parse_visibility(p);

    switch (p->peek.kind) {
        case DK_TOKEN_KIND_PROC: {
            symbol->kind = DK_AST_SYMBOL_KIND_PROC;
            symbol->proc = dk_parse_proc(p);
        } break;

        default: {
            assert(false && "Not implemented");
        } break;
    }

    return symbol;
}

static dk_ast dk_ast_from_str(str s) {
    dk_ast ast = { 0 };

    dk_parser p = { 0 };
    dk_parser_init(&p, s);

    while (p.peek.kind != DK_TOKEN_KIND_EOF) {
        if (dk_global_err->err_list.count > 0) break;

        dk_ast_symbol *symbol = dk_parse_symbol(&p);
        slist_add(&ast.symbols, symbol);
    }

    return ast;
}

////////////////////////////////////////////////////////////////
//
//
// Symbol table
//
//
////////////////////////////////////////////////////////////////

static dk_symbol *dk_symbol_table_add(dk_symbol_table *table, str name) {
    // TODO(rune): Check for duplicate symbol name.

    if (table->slot_count + 1 > table->slot_capacity) {
        u64 next_capacity = table->slot_capacity * 2;
        if (next_capacity == 0) {
            next_capacity = 1024;
        }

        table->slots = heap_realloc(table->slots, sizeof(dk_symbol) * next_capacity, false);
        table->slot_capacity = next_capacity;
    }

    dk_symbol *symbol = &table->slots[table->slot_count];
    symbol->name      = name;

    table->slot_count += 1;

    return symbol;
}

////////////////////////////////////////////////////////////////
//
//
// Type check
//
//
////////////////////////////////////////////////////////////////

static readonly dk_symbol dk_nil_symbol = {
    .type = &dk_nil_symbol,
};

static dk_symbol *dk_check_symbol(dk_checker *checker, str name) {
    dk_symbol *sym = null;

    //- Check local symbols.
    if (sym == null) {
        for_count (dk_symbol, it, checker->local_symbols.slots, checker->local_symbols.slot_count) {
            if (str_eq_nocase(it->name, name)) {
                sym = it;
                break;
            }
        }
    }

    //- Check global symbols.
    if (sym == null) {
        for_count (dk_symbol, it, checker->global_symbols.slots, checker->global_symbols.slot_count) {
            if (str_eq_nocase(it->name, name)) {
                sym = it;
                break;
            }
        }
    }

    //- Undeclared symbol.
    if (sym == null) {
        dk_report_err(dk_global_err, dk_tprint("Undeclared symbol '%'", name)); // TODO(rune): Better error message.
        sym = &dk_nil_symbol;
    }

    //- Has proc been checked yet?
    if (sym->kind == DK_SYMBOL_KIND_PROC && sym->type == null) {
        if (sym->id < 0xdeadbeef) { // TODO(rune): Cleanup builtins.
            assert(sym->ast->kind == DK_AST_SYMBOL_KIND_PROC);
            sym->type = dk_check_symbol(checker, sym->ast->proc.return_type_name);
        }
    }

    return sym;
}

static dk_symbol *dk_check_expr(dk_checker *checker, dk_ast_expr *expr) {
    dk_symbol *ret = &dk_nil_symbol;

    switch (expr->kind) {
        case DK_AST_EXPR_KIND_CALL: {
            dk_symbol *callee = dk_check_symbol(checker, expr->call.ident);
            ret = callee->type;

            if (callee->id < 0xdeadbeef) {
                assert(callee->ast->kind == DK_AST_SYMBOL_KIND_PROC);
                dk_ast_proc *proc = &callee->ast->proc;

                if (expr->call.args.count == callee->ast->proc.args.count) {
                    dk_ast_expr_arg *expr_arg = expr->call.args.first;
                    dk_ast_proc_arg *proc_arg = proc->args.first;

                    for_n (u64, i, expr->call.args.count) {
                        dk_symbol *expr_arg_type = dk_check_expr(checker, expr_arg->expr)->type;
                        dk_symbol *proc_arg_type = proc_arg->symbol->type;

                        if (expr_arg_type != proc_arg_type) {
                            dk_report_err(dk_global_err, dk_tprint("Incompatibles types in argument %.", i + 1)); // TODO(rune): Better error message.
                        }

                        expr_arg = expr_arg->next;
                        proc_arg = proc_arg->next;
                    }

                } else {
                    dk_report_err(dk_global_err, str("Incorrect number of arguments.")); // TODO(rune): Better error message.
                }
            } else {
                // TODO(rune): Actual typecheck builtins.
                for_list (dk_ast_expr_arg, arg, expr->call.args) {
                    dk_check_expr(checker, arg->expr);
                }
            }
        } break;

        case DK_AST_EXPR_KIND_UNARY: {
            assert(false && "Not implemented.");
        } break;

        case DK_AST_EXPR_KIND_BINARY: {
            dk_check_expr(checker, expr->binary.lhs);
            dk_check_expr(checker, expr->binary.rhs);

            dk_symbol *lhs_type = expr->binary.lhs->sym->type;
            dk_symbol *rhs_type = expr->binary.rhs->sym->type;

            switch (expr->binary.operator) {
                case DK_TOKEN_KIND_OR:
                case DK_TOKEN_KIND_AND: {
                    if (lhs_type->type_kind != DK_TYPE_KIND_BOOL) dk_report_err(dk_global_err, str("Incompatible types: Left-hand-side is not boolean.")); // TODO(rune): Better error message.
                    if (rhs_type->type_kind != DK_TYPE_KIND_BOOL) dk_report_err(dk_global_err, str("Incompatible types: Right-hand-side is not boolean.")); // TODO(rune): Better error message.

                    ret = dk_check_symbol(checker, str("påstand"));
                } break;

                case DK_TOKEN_KIND_ADD:
                case DK_TOKEN_KIND_SUB:
                case DK_TOKEN_KIND_MUL:
                case DK_TOKEN_KIND_DIV: {
                    if (lhs_type != rhs_type) dk_report_err(dk_global_err, str("Incompatibles types in binary operator.")); // TODO(rune): Better error message.

                    ret = lhs_type;
                } break;


                case DK_TOKEN_KIND_LESS:
                case DK_TOKEN_KIND_GREATER: {
                    if (lhs_type != rhs_type) dk_report_err(dk_global_err, str("Incompatibles types in comparison operator.")); // TODO(rune): Better error message.

                    ret = dk_check_symbol(checker, str("påstand"));
                } break;

                case DK_TOKEN_KIND_CALL: {

                } break;

                default: {
                    assert(false && "Invalid operator.");
                } break;
            }
        } break;

        case DK_AST_EXPR_KIND_LITERAL: {
            switch (expr->literal.kind) {
                case DK_AST_LITERAL_KIND_INT:   ret = dk_check_symbol(checker, str("heltal"));  break;
                case DK_AST_LITERAL_KIND_FLOAT: ret = dk_check_symbol(checker, str("flyder"));  break;
                case DK_AST_LITERAL_KIND_BOOL:  ret = dk_check_symbol(checker, str("påstand")); break;
            }
        } break;

        case DK_AST_EXPR_KIND_IDENT: {
            dk_symbol *sym = dk_check_symbol(checker, expr->ident);
            ret = sym;
        } break;
    }

    expr->sym = ret;
    return ret;
}

static void dk_check_stmts(dk_checker *checker, dk_ast_stmts stmts, dk_symbol *return_type) {
    //- Statement.
    for_list (dk_ast_stmt, stmt, stmts) {
        if (dk_global_err->err_list.count) break;

        switch (stmt->kind) {
            case DK_AST_STMT_KIND_DECL: {
                dk_ast_decl *decl = &stmt->decl;
                dk_symbol *type = dk_check_symbol(checker, decl->type_name);
                if (type->type_kind == DK_TYPE_KIND_VOID) {
                    dk_report_err(dk_global_err, str("Cannot declare local as void"));
                }

                dk_symbol *local = dk_symbol_table_add(&checker->local_symbols, decl->ident);
                local->kind = DK_SYMBOL_KIND_LOCAL;
                local->id   = (u32)checker->local_size; // TODO(rune): Cast?
                local->type = type;

                checker->local_size += type->size;
            } break;

            case DK_AST_STMT_KIND_EXPR: {
                dk_ast_expr *expr = stmt->expr;
                expr->sym = dk_check_expr(checker, expr);
            } break;

            case DK_AST_STMT_KIND_ASSIGN: {
                dk_ast_assign *assign = &stmt->assign;
                dk_symbol *lsym = dk_check_expr(checker, assign->left);
                assert(lsym->kind == DK_SYMBOL_KIND_LOCAL); // TODO(rune): Better error reporting.

                dk_symbol *rsym = dk_check_expr(checker, assign->right);
                if (lsym->type != rsym->type) {
                    dk_report_err(dk_global_err, str("Incompatible types")); // TODO(rune): Better error message.
                }
            } break;

            case DK_AST_STMT_KIND_RETURN: {
                dk_symbol *sym = dk_check_expr(checker, stmt->return_.expr);
                dk_symbol *type = sym->type;

                if (type != return_type) {
                    dk_report_err(dk_global_err, str("Incompatible return type")); // TODO(rune): Better error message.
                }
            } break;

            case DK_AST_STMT_KIND_IF: {
                dk_symbol *expr = dk_check_expr(checker, stmt->if_.expr);
                if (expr->type->type_kind != DK_TYPE_KIND_BOOL) {
                    dk_report_err(dk_global_err, str("Condition in if statement does not evalutate to a boolean")); // TODO(rune): Better error message.
                }

                dk_check_stmts(checker, stmt->if_.then, return_type);
                dk_check_stmts(checker, stmt->if_.else_, return_type);
            } break;

            case DK_AST_STMT_KIND_WHILE: {
                dk_symbol *expr = dk_check_expr(checker, stmt->while_.expr);
                if (expr->type->type_kind != DK_TYPE_KIND_BOOL) {
                    dk_report_err(dk_global_err, str("Condition in while statement does not evalutate to a boolean")); // TODO(rune): Better error message.
                }

                dk_check_stmts(checker, stmt->while_.body, return_type);
            } break;


            default: {
                assert(false && "Invalid expression kind");
            } break;
        }
    }
}

static void dk_check_proc(dk_checker *checker, dk_symbol *symbol) {
    dk_ast_proc *proc = &symbol->ast->proc;
    proc->type = dk_check_symbol(checker, proc->return_type_name);

    u64 local_size = 0;

    //- Arguments.
    for_list (dk_ast_proc_arg, arg, proc->args) {
        dk_symbol *arg_symbol = dk_symbol_table_add(&checker->local_symbols, arg->name);
        arg_symbol->kind = DK_SYMBOL_KIND_LOCAL;
        arg_symbol->id   = (u32)local_size; // TODO(rune): Cast?
        arg_symbol->type = dk_check_symbol(checker, arg->type_name);

        local_size += arg_symbol->type->size;

        arg->symbol = arg_symbol;
    }

    // NOTE(rune): Statements are checked in a later pass, since we need to know
    // the signature of all procs, before we kan check statements.
}

static dk_symbol_table dk_check_ast(dk_ast ast) {
    dk_checker checker = { 0 };

    // TODO(rune): Better system for built-in types.
    for_array (dk_type, base_type, dk_base_types) {
        dk_symbol *sym = dk_symbol_table_add(&checker.global_symbols, base_type->name);
        sym->id        = ++checker.type_id_counter;
        sym->kind      = DK_SYMBOL_KIND_TYPE;
        sym->size      = base_type->size;
        sym->name      = base_type->name;
        sym->type_kind = base_type->kind;
        sym->type      = sym;
    }

    // TODO(rune): Better system for built-in procs.
    {
        dk_symbol *proc = dk_symbol_table_add(&checker.global_symbols, str("print"));
        proc->id = 0xdeadbeef;
        proc->kind = DK_SYMBOL_KIND_PROC;
        proc->builtin = true;
        proc->type = dk_check_symbol(&checker, str("heltal"));
    }

    // TODO(rune): Better system for built-in procs.
    {
        dk_symbol *proc = dk_symbol_table_add(&checker.global_symbols, str("printflyder"));
        proc->id = 0xdeadbeef + 1;
        proc->kind = DK_SYMBOL_KIND_PROC;
        proc->builtin = true;
        proc->type = dk_check_symbol(&checker, str("heltal"));
    }

    for_list (dk_ast_symbol, ast_symbol, ast.symbols) {
        switch (ast_symbol->kind) {
            case DK_AST_SYMBOL_KIND_PROC: {
                dk_symbol *sym = dk_symbol_table_add(&checker.global_symbols, ast_symbol->proc.ident);
                sym->id = ++checker.proc_id_counter;
                sym->kind = DK_SYMBOL_KIND_PROC;
                sym->ast = ast_symbol;

                dk_check_proc(&checker, sym);
            } break;

            default: {
                assert(false && "Invalid ast symbol kind.");
            } break;
        }
    }

    for_count (dk_symbol, symbol, checker.global_symbols.slots, checker.global_symbols.slot_count) {
        if (symbol->builtin == false) {
            switch (symbol->kind) {
                case DK_SYMBOL_KIND_PROC: {
                    dk_ast_proc *proc = &symbol->ast->proc;
                    dk_check_stmts(&checker, proc->stmts, proc->type);
                } break;

                case DK_SYMBOL_KIND_TYPE: {
                    // TODO(rune): Structs.
                } break;

                default: {
                    assert(false && "Invalid ast symbol kind.");
                } break;
            }
        }
    }

    return checker.global_symbols;
}

////////////////////////////////////////////////////////////////
//
//
// Buffer
//
//
////////////////////////////////////////////////////////////////

static void *dk_buffer_push(dk_buffer *buffer, u64 size) {
    if (buffer->size + size > buffer->capacity) {
        u64 next_size = buffer->capacity * 2;
        if (next_size == 0) {
            next_size = kilobytes(4);
        }

        buffer->data = heap_realloc(buffer->data, next_size, false);
        buffer->capacity = next_size;
    }

    assert(buffer->size + size <= buffer->capacity);
    void *ret = buffer->data + buffer->size;
    buffer->size += size;
    return ret;
}

static void *dk_buffer_pop(dk_buffer *buffer, u64 size) {
    assert(size <= buffer->size); // TODO(rune): Error handling.
    buffer->size -= size;
    void *ret = buffer->data + buffer->size;
    return ret;
}

static void *dk_buffer_get(dk_buffer *buffer, u64 pos, u64 size) {
    assert(pos + size <= buffer->size); // TODO(rune): Error handling.
    void *ret = buffer->data + pos;
    return ret;
}

static void *dk_buffer_read(dk_buffer *buffer, u64 *pos, u64 size) {
    assert(*pos + size <= buffer->size); // TODO(rune): Error handling.
    void *ret = buffer->data + *pos;
    *pos += size;
    return ret;
}

static u8 *dk_buffer_push_u8(dk_buffer *buffer, u8 a) { u8 *b = dk_buffer_push(buffer, sizeof(u8)); *b = a; return b; }
static u16 *dk_buffer_push_u16(dk_buffer *buffer, u16 a) { u16 *b = dk_buffer_push(buffer, sizeof(u16)); *b = a; return b; }
static u32 *dk_buffer_push_u32(dk_buffer *buffer, u32 a) { u32 *b = dk_buffer_push(buffer, sizeof(u32)); *b = a; return b; }
static u64 *dk_buffer_push_u64(dk_buffer *buffer, u64 a) { u64 *b = dk_buffer_push(buffer, sizeof(u64)); *b = a; return b; }

static u8 dk_buffer_pop_u8(dk_buffer *buffer) { return *(u8 *)dk_buffer_pop(buffer, sizeof(u8)); }
static u16 dk_buffer_pop_u16(dk_buffer *buffer) { return *(u16 *)dk_buffer_pop(buffer, sizeof(u16)); }
static u32 dk_buffer_pop_u32(dk_buffer *buffer) { return *(u32 *)dk_buffer_pop(buffer, sizeof(u32)); }
static u64 dk_buffer_pop_u64(dk_buffer *buffer) { return *(u64 *)dk_buffer_pop(buffer, sizeof(u64)); }

static u8 *dk_buffer_get_u8(dk_buffer *buffer, u64 pos) { return (u8 *)dk_buffer_get(buffer, pos, sizeof(u8)); }
static u16 *dk_buffer_get_u16(dk_buffer *buffer, u64 pos) { return (u16 *)dk_buffer_get(buffer, pos, sizeof(u16)); }
static u32 *dk_buffer_get_u32(dk_buffer *buffer, u64 pos) { return (u32 *)dk_buffer_get(buffer, pos, sizeof(u32)); }
static u64 *dk_buffer_get_u64(dk_buffer *buffer, u64 pos) { return (u64 *)dk_buffer_get(buffer, pos, sizeof(u64)); }

static u8 dk_buffer_read_u8(dk_buffer *buffer, u64 *pos) { return *(u8 *)dk_buffer_read(buffer, pos, sizeof(u8)); }
static u16 dk_buffer_read_u16(dk_buffer *buffer, u64 *pos) { return *(u16 *)dk_buffer_read(buffer, pos, sizeof(u16)); }
static u32 dk_buffer_read_u32(dk_buffer *buffer, u64 *pos) { return *(u32 *)dk_buffer_read(buffer, pos, sizeof(u32)); }
static u64 dk_buffer_read_u64(dk_buffer *buffer, u64 *pos) { return *(u64 *)dk_buffer_read(buffer, pos, sizeof(u64)); }

////////////////////////////////////////////////////////////////
//
//
// Emit
//
//
////////////////////////////////////////////////////////////////

static void dk_compiler_add_local(dk_compiler_local_list *locals, str name, str type) {
    if (locals->count >= U8_MAX) {
        // TODO(rune): Better error meessage.
        // TODO(rune): Test.
        dk_report_err(dk_global_err, str("Too many locals."));
    }

    dk_compiler_local *local = arena_push_struct(&dk_bc_arena, dk_compiler_local);
    local->off = locals->count; // TODO(rune): Type specific.
    local->name = name;
    local->type = type;

    slist_add(locals, local);
    locals->count += 1;
}

static u16 dk_compiler_get_local(dk_compiler_local_list locals, str name) {
    for_list (dk_compiler_local, local, locals) {
        if (str_eq_nocase(local->name, name)) {
            return local->off;
        }
    }

    dk_report_err(dk_global_err, str("Undeclared local."));
    return 0;
}

static dk_symbol *dk_resolve_symbol(dk_emitter *e, str name) {
    dk_symbol_table *table = &e->symbol_table;

    dk_symbol *symbol = null;
    for_n (u64, i, table->slot_count) {
        if (str_eq_nocase(table->slots[i].name, name)) {
            symbol = &table->slots[i];
            break;
        }
    }

    if (symbol == null) {
        dk_report_err(dk_global_err, dk_tprint("Unresolved symbol '%'.", name)); // TODO(rune): Better error message.
    }

    return symbol;
}

static u32 dk_resolve_symbol_id(dk_emitter *e, str name) {
    dk_symbol *symbol = dk_resolve_symbol(e, name);
    assert(symbol->id != 0);
    return symbol->id;
}

static u8 *dk_emit_u8(dk_emitter *e, u8 a) { return dk_buffer_push_u8(&e->body, a); }
static u16 *dk_emit_u16(dk_emitter *e, u16 a) { return dk_buffer_push_u16(&e->body, a); }
static u32 *dk_emit_u32(dk_emitter *e, u32 a) { return dk_buffer_push_u32(&e->body, a); }
static u64 *dk_emit_u64(dk_emitter *e, u64 a) { return dk_buffer_push_u64(&e->body, a); }

static void dk_emit_inst1(dk_emitter *e, dk_bc_opcode opcode) {
    assert(dk_bc_opcode_infos[opcode].operand_kind == DK_BC_OPERAND_KIND_NONE);

    dk_bc_inst_prefix prefix = { .opcode = opcode };
    dk_emit_u8(e, prefix.u8);
}

static void dk_emit_inst2(dk_emitter *e, dk_bc_opcode opcode, u64 operand) {
    assert(dk_bc_opcode_infos[opcode].operand_kind != DK_BC_OPERAND_KIND_NONE);

    dk_bc_inst_prefix prefix = { .opcode = opcode };
    if (operand <= U8_MAX) {
        prefix.operand_size = 0;
        dk_emit_u8(e, prefix.u8);
        dk_emit_u8(e, (u8)operand);
    } else if (operand <= U16_MAX) {
        prefix.operand_size = 1;
        dk_emit_u8(e, prefix.u8);
        dk_emit_u16(e, (u16)operand);
    } else if (operand <= U32_MAX) {
        prefix.operand_size = 2;
        dk_emit_u8(e, prefix.u8);
        dk_emit_u32(e, (u32)operand);
    } else {
        prefix.operand_size = 3;
        dk_emit_u8(e, prefix.u8);
        dk_emit_u64(e, (u64)operand);
    }
}

static void dk_emit_symbol(dk_emitter *e, u32 id, u32 size) {
    dk_bc_symbol *symbol = dk_buffer_push_struct(&e->head, dk_bc_symbol);
    symbol->id   = id;
    symbol->size = size;
    symbol->pos  = e->body.size;
}

static void dk_emit_literal(dk_emitter *e, dk_ast_literal *literal) {
    switch (literal->kind) {
        case DK_AST_LITERAL_KIND_INT:   dk_emit_inst2(e, DK_BC_OPCODE_LDI, u64(literal->i64_));         break;
        case DK_AST_LITERAL_KIND_FLOAT: dk_emit_inst2(e, DK_BC_OPCODE_LDI, f64_as_u64(literal->f64_));  break;
        case DK_AST_LITERAL_KIND_BOOL:  dk_emit_inst2(e, DK_BC_OPCODE_LDI, u64(literal->boolean));      break;
        default:                        assert(false && "Not implemented.");                            break;
    }
}

static void dk_emit_expr(dk_emitter *e, dk_ast_expr *expr, dk_compiler_local_list *locals) {
    switch (expr->kind) {
        case DK_AST_EXPR_KIND_CALL: {
            for_list (dk_ast_expr_arg, arg, expr->call.args) {
                dk_emit_expr(e, arg->expr, locals);
            }

            dk_emit_inst2(e, DK_BC_OPCODE_CALL, dk_resolve_symbol_id(e, expr->call.ident));
        } break;

        case DK_AST_EXPR_KIND_UNARY: {
            assert(false && "Not implemented.");
        } break;

        case DK_AST_EXPR_KIND_BINARY: {
            dk_emit_expr(e, expr->binary.lhs, locals);
            dk_emit_expr(e, expr->binary.rhs, locals);

            bool is_float = expr->sym->type->type_kind == DK_TYPE_KIND_F64;
            switch (expr->binary.operator) {
                case DK_TOKEN_KIND_ADD: is_float ? dk_emit_inst1(e, DK_BC_OPCODE_FADD) : dk_emit_inst1(e, DK_BC_OPCODE_ADD);  break;
                case DK_TOKEN_KIND_SUB: is_float ? dk_emit_inst1(e, DK_BC_OPCODE_FSUB) : dk_emit_inst1(e, DK_BC_OPCODE_SUB);  break;
                case DK_TOKEN_KIND_MUL: is_float ? dk_emit_inst1(e, DK_BC_OPCODE_FMUL) : dk_emit_inst1(e, DK_BC_OPCODE_IMUL); break; // TODO(rune): Unsigned multiply.
                case DK_TOKEN_KIND_DIV: is_float ? dk_emit_inst1(e, DK_BC_OPCODE_FDIV) : dk_emit_inst1(e, DK_BC_OPCODE_IDIV); break; // TODO(rune): Unsigned divide.

                case DK_TOKEN_KIND_AND:     dk_emit_inst1(e, DK_BC_OPCODE_AND);  break;
                case DK_TOKEN_KIND_OR:      dk_emit_inst1(e, DK_BC_OPCODE_OR);   break;
                case DK_TOKEN_KIND_LESS:    dk_emit_inst1(e, DK_BC_OPCODE_LT);   break;
                case DK_TOKEN_KIND_GREATER: dk_emit_inst1(e, DK_BC_OPCODE_GT);   break;

                default: {
                    assert(false && "Invalid operator.");
                } break;
            }
        } break;

        case DK_AST_EXPR_KIND_LITERAL: {
            // TODO(rune): Push actual literal value.
            dk_emit_literal(e, &expr->literal);
        } break;

        case DK_AST_EXPR_KIND_IDENT: {
            dk_emit_inst2(e, DK_BC_OPCODE_LDL, dk_compiler_get_local(*locals, expr->ident));
        } break;

        default: {
            assert(false && "Invalid expr kind.");
        } break;

    }
}

// TODO(rune): Per stmt-block local scopes. Currently all locals are scoped to the whole procedure,
// but they should be scoped to a block instead.
static void dk_emit_stmts(dk_emitter *e, dk_ast_stmts stmts, dk_compiler_local_list *locals) {
    for_list (dk_ast_stmt, stmt, stmts) {
        if (dk_global_err->err_list.count > 0) {
            break;
        }

        switch (stmt->kind) {
            case DK_AST_STMT_KIND_DECL: {
                dk_compiler_add_local(locals, stmt->decl.ident, stmt->decl.type_name);
            } break;

            case DK_AST_STMT_KIND_EXPR: {
                dk_emit_expr(e, stmt->expr, locals);

                // NOTE(rune): Stack cleanup if expr value wasn't popped by an assignment.
                if (stmt->expr->sym->type->type_kind != DK_TYPE_KIND_VOID) {
                    dk_emit_inst1(e, DK_BC_OPCODE_POP);
                }
            } break;

            case DK_AST_STMT_KIND_ASSIGN: {
                dk_ast_expr *left  = stmt->assign.left;
                dk_ast_expr *right = stmt->assign.right;

                dk_emit_expr(e, right, locals);

                switch (left->kind) {
                    case DK_AST_EXPR_KIND_IDENT: {
                        switch (left->sym->kind) {
                            case DK_SYMBOL_KIND_LOCAL: {
                                dk_emit_inst2(e, DK_BC_OPCODE_STL, dk_compiler_get_local(*locals, left->ident));
                            } break;

                            default: {
                                assert(false && "Invalid symbol kind.");
                            } break;
                        }
                    } break;

                    default: {
                        assert(false && "Invalid expression kind.");
                    } break;
                }

            } break;

            case DK_AST_STMT_KIND_RETURN: {
                if (stmt->return_.expr) {
                    dk_emit_expr(e, stmt->return_.expr, locals);
                }

                dk_emit_inst1(e, DK_BC_OPCODE_RET);
            } break;

            case DK_AST_STMT_KIND_IF: {
                dk_emit_expr(e, stmt->if_.expr, locals);
                dk_emit_inst1(e, DK_BC_OPCODE_NOT);

                // TODO(rune): This whole business is a hack.

                dk_bc_inst_prefix prefix = {
                    .opcode = DK_BC_OPCODE_BR,
                    .operand_size = 3, // NOTE(rune): 64-bits
                };
                dk_emit_u8(e, prefix.u8);
                u64 *else_pos = dk_emit_u64(e, U64_MAX);

                dk_emit_stmts(e, stmt->if_.then, locals);

                dk_emit_inst2(e, DK_BC_OPCODE_LDI, 1);
                dk_bc_inst_prefix prefix2 = {
                    .opcode = DK_BC_OPCODE_BR,
                    .operand_size = 3, // NOTE(rune): 64-bits
                };
                dk_emit_u8(e, prefix2.u8);

                u64 *end_pos = dk_emit_u64(e, U64_MAX);

                *else_pos = e->body.size;

                dk_emit_stmts(e, stmt->if_.else_, locals);

                *end_pos = e->body.size;
            } break;

            case DK_AST_STMT_KIND_WHILE: {
                u64 start_pos = e->body.size;
                dk_emit_expr(e, stmt->while_.expr, locals);
                dk_emit_inst1(e, DK_BC_OPCODE_NOT);

                // TODO(rune): This whole business is a hack.


                dk_bc_inst_prefix prefix = {
                    .opcode = DK_BC_OPCODE_BR,
                    .operand_size = 3, // NOTE(rune): 64-bits
                };
                dk_emit_u8(e, prefix.u8);
                u64 *end_pos = dk_emit_u64(e, U64_MAX);

                dk_emit_stmts(e, stmt->while_.body, locals);

                dk_emit_inst2(e, DK_BC_OPCODE_LDI, 1); // TODO(rune): Unconditional jump opcode.
                dk_emit_inst2(e, DK_BC_OPCODE_BR, start_pos);

                *end_pos = e->body.size;
            } break;

            default: {
                assert(false && "Invalid stmt kind.");
            } break;
        }
    }

}

static void dk_emit_proc(dk_emitter *e, dk_ast_proc *proc) {
    dk_compiler_local_list locals = { 0 };

    //- Prologue.
    for_list (dk_ast_proc_arg, arg, proc->args) {
        dk_compiler_add_local(&locals, arg->name, arg->type_name);
        dk_emit_inst2(e, DK_BC_OPCODE_STL, dk_compiler_get_local(locals, arg->name));
    }

    //- Body.
    dk_emit_stmts(e, proc->stmts, &locals);

    //- Epilogue.
    dk_emit_inst1(e, DK_BC_OPCODE_RET);
}

static dk_program dk_program_from_ast(dk_ast ast) {
    dk_emitter e = { 0 };

#if DK_DEBUG_PRINT_PARSE
    dk_print_ast(ast);
#endif
    e.symbol_table = dk_check_ast(ast);

    for_list (dk_ast_symbol, symbol, ast.symbols) {
        switch (symbol->kind) {
            case DK_AST_SYMBOL_KIND_PROC: {
                dk_emit_symbol(&e, dk_resolve_symbol_id(&e, symbol->proc.ident), 64); // TODO(rune): Properly calculate size of locals.
                dk_emit_proc(&e, &symbol->proc);
            } break;

            default: {
                assert(false && "Invalid symbol kind.");
            } break;
        }
    }

    dk_program program = {
        .body = e.body,
        .head = e.head,
    };

#if DK_DEBUG_PRINT_EMIT
    dk_print_program(program);
#endif

    return program;
}

static dk_program dk_program_from_str(str s, dk_err_sink *err_sink) {
    dk_global_err = err_sink;

    if (err_sink->err_list.count) return (dk_program) { 0 };
    dk_ast ast = dk_ast_from_str(s);

    if (err_sink->err_list.count) return (dk_program) { 0 };
    dk_program program = dk_program_from_ast(ast);

    return program;
}

////////////////////////////////////////////////////////////////
//
//
// Runtime
//
//
////////////////////////////////////////////////////////////////

static str dk_run_program(dk_program program, arena *output_arena) {
    typedef struct dk_call_frame dk_call_frame;
    struct dk_call_frame {
        u64 loc_base;
        u64 loc_size;
        u64 return_pos;
        dk_call_frame *prev;
    };

    u64 ip = 0;

    str_list output_list = { 0 };

    dk_buffer *head = &program.head;
    dk_buffer *body = &program.body;

    dk_bc_symbol *symbols = (dk_bc_symbol *)head->data;
    u64 symbol_count      = head->size / sizeof(dk_bc_symbol);

    dk_buffer data_stack = { 0 };
    dk_buffer call_stack = { 0 };

    //- Setup initial call frame.
    u64 hardcoded_local_size = 64; // TODO(rune): Encode number of locals and arguments.
    dk_call_frame *frame = dk_buffer_push_struct(&call_stack, dk_call_frame);
    frame->return_pos = U64_MAX;
    frame->loc_base = call_stack.size;
    frame->loc_size = hardcoded_local_size;

    dk_buffer_push(&call_stack, hardcoded_local_size);

    while (1) {
        dk_bc_inst_prefix prefix = *dk_buffer_read_struct(body, &ip, dk_bc_inst_prefix);
        dk_bc_opcode opcode      = prefix.opcode;
        u64 operand              = 0;

        if (dk_bc_opcode_infos[prefix.opcode].operand_kind != DK_BC_OPERAND_KIND_NONE) {
            switch (prefix.operand_size) {
                case 0: operand = (u64)dk_buffer_read_u8(body, &ip); break;
                case 1: operand = (u64)dk_buffer_read_u16(body, &ip); break;
                case 2: operand = (u64)dk_buffer_read_u32(body, &ip); break;
                case 3: operand = (u64)dk_buffer_read_u64(body, &ip); break;
                default: {
                    assert(false); // TODO(rune): Better error runtime error reporting.
                } break;
            }
        }

        assert(opcode < DK_BC_OPCODE_COUNT); // TODO(rune): Better error runtime error reporting.

        switch (prefix.opcode) {
            case DK_BC_OPCODE_NOP: { } break;

            case DK_BC_OPCODE_LDI: {
                dk_buffer_push_u64(&data_stack, operand);
            } break;

            case DK_BC_OPCODE_LDL: {
                u64 loc_val = *dk_buffer_get_u64(&call_stack, frame->loc_base + operand * 8); // TODO(rune): Properly sized locals (no more *8)
                dk_buffer_push_u64(&data_stack, loc_val);
            } break;

            case DK_BC_OPCODE_POP: {
                dk_buffer_pop_u64(&data_stack);
            } break;

            case DK_BC_OPCODE_STL: {
                u64 val = dk_buffer_pop_u64(&data_stack);
                *dk_buffer_get_u64(&call_stack, frame->loc_base + operand * 8) = val; // TODO(rune): Properly sized locals (no more *8)
            } break;

#define DK_BC_BINOP_IMPL(calc)                          \
            do {                                        \
                u64 b = dk_buffer_pop_u64(&data_stack); \
                u64 a = dk_buffer_pop_u64(&data_stack); \
                u64 c = calc;                           \
                dk_buffer_push_u64(&data_stack, c);     \
            } while (0)

            case DK_BC_OPCODE_ADD:  DK_BC_BINOP_IMPL(a + b); break;
            case DK_BC_OPCODE_SUB:  DK_BC_BINOP_IMPL(a - b); break;
            case DK_BC_OPCODE_UMUL: DK_BC_BINOP_IMPL(a * b); break;
            case DK_BC_OPCODE_UDIV: DK_BC_BINOP_IMPL(a / b); break;

            case DK_BC_OPCODE_IMUL: DK_BC_BINOP_IMPL(u64(i64(a) * i64(b))); break;
            case DK_BC_OPCODE_IDIV: DK_BC_BINOP_IMPL(u64(i64(a) / i64(b))); break;

            case DK_BC_OPCODE_FADD: DK_BC_BINOP_IMPL(f64_as_u64(u64_as_f64(a) + u64_as_f64(b))); break;
            case DK_BC_OPCODE_FSUB: DK_BC_BINOP_IMPL(f64_as_u64(u64_as_f64(a) - u64_as_f64(b))); break;
            case DK_BC_OPCODE_FMUL: DK_BC_BINOP_IMPL(f64_as_u64(u64_as_f64(a) * u64_as_f64(b))); break;
            case DK_BC_OPCODE_FDIV: DK_BC_BINOP_IMPL(f64_as_u64(u64_as_f64(a) / u64_as_f64(b))); break;

            case DK_BC_OPCODE_AND:  DK_BC_BINOP_IMPL(a && b); break;
            case DK_BC_OPCODE_OR:   DK_BC_BINOP_IMPL(a || b); break;

            case DK_BC_OPCODE_LT:   DK_BC_BINOP_IMPL(a < b); break;
            case DK_BC_OPCODE_GT:   do {
                u64 b = dk_buffer_pop_u64(&data_stack);
                u64 a = dk_buffer_pop_u64(&data_stack);
                u64 c = a > b; dk_buffer_push_u64(&data_stack, c);
                printf("");
            } while (0); break;

#undef DK_BC_BINOP_IMPL

            case DK_BC_OPCODE_NOT: {
                u64 a = dk_buffer_pop_u64(&data_stack);
                u64 c = a ? 0 : 1;
                dk_buffer_push_u64(&data_stack, c);
            } break;

            case DK_BC_OPCODE_CALL: {
                u32 id = (u32)operand;

                // TODO(rune): Better system for built-in procs.
                if (id == 0xdeadbeef) {
                    u64 a = dk_buffer_pop_u64(&data_stack);
                    str_list_push_fmt(&output_list, output_arena, "%\n", a);
                    dk_buffer_push_u64(&data_stack, 0); // TODO(rune): What should print return?
                } else if (id == 0xdeadbeef + 1) {
                    u64 a = dk_buffer_pop_u64(&data_stack);
                    str_list_push_fmt(&output_list, output_arena, "%\n", u64_as_f64(a));
                    dk_buffer_push_u64(&data_stack, 0); // TODO(rune): What should print return?
                } else {
                    // TODO(rune): Better symbol lookup.
                    dk_bc_symbol *symbol = null;
                    for_n (u64, i, symbol_count) {
                        if (symbols[i].id == id) {
                            symbol = &symbols[i];
                        }
                    }
                    assert(symbol != null); // TODO(rune): Better error runtime error reporting.

                    dk_call_frame *next_frame = dk_buffer_push_struct(&call_stack, dk_call_frame);
                    next_frame->prev  = frame;
                    frame             = next_frame;
                    frame->loc_base   = call_stack.size;
                    frame->loc_size   = hardcoded_local_size;
                    frame->return_pos = ip;

                    ip = symbol->pos;

                    dk_buffer_push(&call_stack, frame->loc_size);
                }

            } break;

            case DK_BC_OPCODE_RET: {
                if (frame->return_pos == U64_MAX) {
                    goto exit;
                }

                ip = frame->return_pos;;
                dk_buffer_pop(&call_stack, frame->loc_size);
                dk_buffer_pop_struct(&call_stack, dk_call_frame);
                frame = frame->prev;

                assert(frame != null);
            } break;

            case DK_BC_OPCODE_BR: {
                u64 condition = dk_buffer_pop_u64(&data_stack);
                if (condition) {
                    ip = operand;
                }
            } break;

            default: {
                assert(false && "Invalid instruction.");
            } break;
        }
    }

exit:
    if (data_stack.size != 8) {
        str_list_push(&output_list, output_arena, dk_tprint("Invalid stack size on exit. Was % but expected %.", data_stack.size, 8));
    }

    str output = str_list_join(&output_list, output_arena, str(""));
    return output;
}

////////////////////////////////////////////////////////////////
//
//
// Debug printing
//
//
////////////////////////////////////////////////////////////////

static void dk_print_token(dk_token *token) {
    // TODO(rune): Print filename
    print("%:%\t", token->row + 1, token->col + 1);
    print(dk_token_kind_as_str(token->kind));
    print("\t");
    print(token->text);
    print("\n");
}

static void dk_print_ast_level(u64 level) {
    static readonly str colors[] = {
        STR(ANSICONSOLE_FG_BLUE),
        STR(ANSICONSOLE_FG_MAGENTA),
        STR(ANSICONSOLE_FG_RED),
        STR(ANSICONSOLE_FG_YELLOW),
        STR(ANSICONSOLE_FG_GREEN),
        STR(ANSICONSOLE_FG_CYAN),
    };

    for_n (u64, i, level) {
        print(colors[i % countof(colors)]);

        if (i + 1 < level) {
            print("│ ");
        } else {
            print("├─");
        }
    }

    print(ANSICONSOLE_FG_DEFAULT);
}

static void dk_print_ast_literal(dk_ast_literal *literal, u64 level) {
    dk_print_ast_level(level);
    switch (literal->kind) {
        case DK_AST_LITERAL_KIND_INT:   println("literal/int %", literal->i64_);        break;
        case DK_AST_LITERAL_KIND_FLOAT: println("literal/float %", literal->f64_);      break;
        case DK_AST_LITERAL_KIND_BOOL:  println("literal/bool %", literal->boolean);    break;
        default:                        assert(false && "Invalid literal kind.");       break;
    }
}

static void dk_print_ast_expr(dk_ast_expr *expr, u64 level) {
    dk_print_ast_level(level);

    switch (expr->kind) {
        case DK_AST_EXPR_KIND_CALL: {
            println("expr-call $%", expr->call.ident);
            for_list (dk_ast_expr_arg, arg, expr->call.args) {
                dk_print_ast_level(level + 1);
                println("expr-arg");
                dk_print_ast_expr(arg->expr, level + 2);
            }
        } break;

        case DK_AST_EXPR_KIND_UNARY: {
            assert(false && "Not implemented.");
        } break;

        case DK_AST_EXPR_KIND_BINARY: {
            println("expr-binary %", dk_token_kind_as_str(expr->binary.operator));

            dk_print_ast_level(level + 1);
            println("left");
            dk_print_ast_expr(expr->binary.lhs, level + 2);

            dk_print_ast_level(level + 1);
            println("right");
            dk_print_ast_expr(expr->binary.rhs, level + 2);
        } break;

        case DK_AST_EXPR_KIND_LITERAL: {
            println("expr-literal");
            dk_print_ast_literal(&expr->literal, level + 1);
        } break;

        case DK_AST_EXPR_KIND_IDENT: {
            println("expr-ident $%", expr->ident);
        } break;

        default: {
            assert(false && "Invalid expr kind.");
        } break;
    }
}

static void dk_print_ast_stmt(dk_ast_stmt *stmt, u64 level) {
    dk_print_ast_level(level);

    switch (stmt->kind) {
        case DK_AST_STMT_KIND_DECL: {
            dk_ast_decl *stmt_decl = &stmt->decl;
            println("stmt-decl $% type $%", stmt_decl->ident, stmt_decl->type_name);
            if (stmt_decl->expr) {
                assert(false && "Not implemented.");
            }
        } break;

        case DK_AST_STMT_KIND_EXPR: {
            println("stmt-expr");
            dk_print_ast_expr(stmt->expr, level + 1);
        } break;

        case DK_AST_STMT_KIND_ASSIGN: {
            println("stmt-assign");

            dk_print_ast_level(level + 1);
            println("left");
            dk_print_ast_expr(stmt->assign.left, level + 2);

            dk_print_ast_level(level + 1);
            println("right");
            dk_print_ast_expr(stmt->assign.right, level + 2);
        } break;

        case DK_AST_STMT_KIND_RETURN: {
            println("stmt-return");
            if (stmt->return_.expr) {
                dk_print_ast_expr(stmt->return_.expr, level + 1);
            }
        } break;

        case DK_AST_STMT_KIND_IF: {
            println("stmt-if");

            dk_print_ast_level(level + 1);
            println("expr");
            dk_print_ast_expr(stmt->if_.expr, level + 2);

            dk_print_ast_level(level + 1);
            println("then");
            dk_print_ast_stmts(stmt->if_.then, level + 2);

            if (stmt->if_.else_.first) {
                dk_print_ast_level(level + 1);
                println("else");
                dk_print_ast_stmts(stmt->if_.then, level + 2);
            }
        } break;

        case DK_AST_STMT_KIND_WHILE: {
            println("stmt-while");

            dk_print_ast_level(level + 1);
            println("expr");
            dk_print_ast_expr(stmt->while_.expr, level + 2);

            dk_print_ast_level(level + 1);
            println("body");
            dk_print_ast_stmts(stmt->while_.body, level + 2);
        } break;


        default: {
            assert(false && "Invalid stmt kind.");
        } break;
    }
}

static void dk_print_ast_stmts(dk_ast_stmts stmts, u64 level) {
    for_list (dk_ast_stmt, stmt, stmts) {
        dk_print_ast_stmt(stmt, level);
    }
}

static void dk_print_ast_symbol(dk_ast_symbol *symbol, u64 level) {
    dk_print_ast_level(level);

    switch (symbol->kind) {
        case DK_AST_SYMBOL_KIND_PROC: {
            dk_ast_proc *proc = &symbol->proc;
            println("symbol-proc $% returns $%", proc->ident, proc->return_type_name);

            dk_print_ast_level(level + 1);
            println("args");
            for_list (dk_ast_proc_arg, arg, proc->args) {
                dk_print_ast_level(level + 2);
                println("arg $% type $%", arg->name, arg->type_name);
            }

            dk_print_ast_level(level + 1);
            println("stmts");
            dk_print_ast_stmts(proc->stmts, level + 2);
        } break;

        default: {
            assert(false && "Invalid symbol kind.");
        } break;
    }
}

static void dk_print_ast(dk_ast ast) {
    for_list (dk_ast_symbol, symbol, ast.symbols) {
        dk_print_ast_symbol(symbol, 0);
    }
}

static void dk_print_program(dk_program program) {
    dk_buffer *head = &program.head;
    dk_buffer *body = &program.body;

    //- Head
    {
        u64 read_pos = 0;
        while (read_pos < head->size) {
            dk_bc_symbol *symbol = dk_buffer_read_struct(head, &read_pos, dk_bc_symbol);
            print(ANSICONSOLE_FG_CYAN "%(hexpad)" ANSICONSOLE_FG_DEFAULT " %(hexpad)" ANSICONSOLE_FG_GRAY " %(hexpad)\n", symbol->id, symbol->size, symbol->pos);
        }
    }

    //- Body
    {
        u64 read_pos = 0;
        while (read_pos < body->size) {
            print(ANSICONSOLE_FG_GRAY);
            print("%(hexpad)", read_pos);

            dk_bc_inst_prefix prefix       = *dk_buffer_read_struct(body, &read_pos, dk_bc_inst_prefix);
            dk_bc_opcode opcode            = prefix.opcode;
            dk_bc_opcode_info *opcode_info = &dk_bc_opcode_infos[prefix.opcode];

            print(ANSICONSOLE_FG_YELLOW);
            print(" %\t", opcode_info->name);

            any operand = { 0 };
            if (opcode_info->operand_kind != DK_BC_OPERAND_KIND_NONE) {
                switch (prefix.operand_size) {
                    case 0: operand = anyof(dk_buffer_read_u8(body, &read_pos)); break;
                    case 1: operand = anyof(dk_buffer_read_u16(body, &read_pos)); break;
                    case 2: operand = anyof(dk_buffer_read_u32(body, &read_pos)); break;
                    case 3: operand = anyof(dk_buffer_read_u64(body, &read_pos)); break;
                }

                if (opcode_info->operand_kind == DK_BC_OPERAND_KIND_IMM) print(ANSICONSOLE_FG_MAGENTA "%(hexpad)", operand);
                if (opcode_info->operand_kind == DK_BC_OPERAND_KIND_SYM) print(ANSICONSOLE_FG_CYAN    "%(hexpad)", operand);
                if (opcode_info->operand_kind == DK_BC_OPERAND_KIND_LOC) print(ANSICONSOLE_FG_GREEN   "%(hexpad)", operand);
                if (opcode_info->operand_kind == DK_BC_OPERAND_KIND_POS) print(ANSICONSOLE_FG_GRAY    "%(hexpad)", operand);
            }

            print(ANSICONSOLE_FG_DEFAULT);
            print("\n");
        }
    }
}
