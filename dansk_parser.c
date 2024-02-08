static token make_eof_token() {
    token token = { 0 };
    token.kind = TOKEN_KIND_EOF;
    return token;
}

static token peek_token(parser *parser) {
    token ret = make_eof_token();
    if (parser->token_iter.valid) {
        ret = *parser->token_iter.at;
    } 
    return ret;
}

static token consume_token(parser *parser) {
    token ret = peek_token(parser);

    if (parser->token_iter.valid) {
        token_iter_next(&parser->token_iter);
    }

    return ret;
}

static str peek_string(parser *parser) {
    return peek_token(parser).str;
}

static void parser_exit_error(parser *parser, str s) {
    token t = peek_token(parser);
    println("Parse fejl på linje %, kolonne %", t.linenumber, t.column);
    println("    %", s);
    exit(0);
}

static token expect_and_peek(parser *parser, token_kind expect_kind) {
    if (peek_token(parser).kind != expect_kind) {
        str msg = tprint(u8"Forventede % ved %.", token_kind_as_str(expect_kind), peek_string(parser));
        parser_exit_error(parser, msg);
    }
    return peek_token(parser);
}

static token expect_and_consume(parser *parser, token_kind expect_kind) {
    expect_and_peek(parser, expect_kind);
    return consume_token(parser);
}

static token expect_and_consume_keyword(parser *parser, keyword keyword) {
    token peek = peek_token(parser);
    if (peek.kind != TOKEN_KIND_KEYWORD || peek.keyword != keyword) {
        str msg = tprint(u8"Forventede nøgleord % i stedet for %.", keyword_as_str(keyword), peek_string(parser));
        parser_exit_error(parser, msg);
    }
    return consume_token(parser);
}

static token expect_and_consume_str(parser *parser, str expect_string) {
    if (!str_eq(peek_string(parser), expect_string)) {
        parser_exit_error(parser, tprint(u8"Forventede %.", expect_string));
    }
    return consume_token(parser);
}

static ast_node *push_ast_node(parser *parser, ast_node_kind kind, ast_node *parent) {
    ast_node *node = push_struct(parser->arena, ast_node);
    node->kind = kind;
    node->parent = parent;
    return node;
}

static ast_expr *parse_expr_ident_or_literal(parser *parser) {
    ast_expr *expr = push_struct(parser->arena, ast_expr);
    token t = peek_token(parser);
    switch (t.kind) {
        case TOKEN_KIND_IDENT: {
            consume_token(parser);
            expr->kind = AST_EXPR_KIND_IDENT;
            expr->as_ident.str = t.str;
        } break;

        case TOKEN_KIND_LITERAL: {
            consume_token(parser);
            expr->kind = AST_EXPR_KIND_LITERAL;
            expr->as_literal.value = t.var_value;
        } break;

        default: {
            parser_exit_error(parser, str("Forventede identifikator eller konstant."));
        } break;
    }
    return expr;
}

static ast_expr *parse_expr(parser *parser) {
    ast_expr *ret = null;
    ast_expr *left = parse_expr_ident_or_literal(parser);

    if (peek_token(parser).kind == TOKEN_KIND_OPERATOR) {
        token operator_token = expect_and_consume(parser, TOKEN_KIND_OPERATOR);
        ret = push_struct(parser->arena, ast_expr);
        ret->kind = AST_EXPR_KIND_BINOP;
        ret->as_binop.operator = operator_token.operator;
        ret->as_binop.left = left;
        ret->as_binop.right = parse_expr_ident_or_literal(parser);
    } else {
        ret = left;
    }

    return ret;
}

static ast_node *parse_assign(parser *parser, ast_node *parent) {
    // Example: "Sæt B til A plus en.".
    ast_node *ret = push_ast_node(parser, AST_NODE_KIND_ASSIGN, parent);

    expect_and_consume_keyword(parser, KEYWORD_ASSIGN);

    token l_token = expect_and_consume(parser, TOKEN_KIND_IDENT);
    expect_and_consume_str(parser, str("til"));

    ast_expr *r_expr = parse_expr(parser);

    ret->as_assign.l_ident = l_token.str;
    ret->as_assign.r_expr = r_expr;

    expect_and_consume(parser, TOKEN_KIND_PERIOD);
    return ret;
}

static ast_node *parse_decl(parser *parser, ast_node *parent) {
    // Example: "Lad A være et helttal.".
    ast_node *ret = push_ast_node(parser, AST_NODE_KIND_DECL, parent);

    expect_and_consume_keyword(parser, KEYWORD_DECL);

    var_kind var_kind = 0;
    str ident = str("");

    token ident_token = expect_and_consume(parser, TOKEN_KIND_IDENT);
    ident = ident_token.str;

    expect_and_consume_str(parser, str("være"));

    if (str_eq(peek_string(parser), str("en")) || str_eq(peek_string(parser), str("et"))) {
        token enet = consume_token(parser);

        // @Todo Check en/et grammatik.
        var_kind = VAR_KIND_NONE;
        str var_kind_name = peek_string(parser);
        if (str_eq(var_kind_name, str("flyder"))) {
            var_kind = VAR_KIND_F64;
            consume_token(parser);
        } else if (str_eq(var_kind_name, str("heltal"))) {
            var_kind = VAR_KIND_I64;
            consume_token(parser);
        } else {
            parser_exit_error(parser, tprint("Ukendt type '%'", var_kind_name));
        }
    } else {
        parser_exit_error(parser, str("Forventede 'en' eller 'et'."));
    }

    expect_and_consume(parser, TOKEN_KIND_PERIOD);

    ret->as_decl.var_kind = var_kind;
    ret->as_decl.ident = ident;
    return ret;
}

static ast_node *parse_output(parser *parser, ast_node *parent) {
    // Example: "Udput B."

    ast_node *ret = push_ast_node(parser, AST_NODE_KIND_OUTPUT, parent);

    expect_and_consume_keyword(parser, KEYWORD_OUTPUT);
    ret->as_output.expr = parse_expr(parser);

    expect_and_consume(parser, TOKEN_KIND_PERIOD);

    return ret;
}

static ast_node *parse_node(parser *parser, ast_node *parent);

static ast_node *parse_block(parser *parser, ast_node *parent) {
    ast_node *ret = push_ast_node(parser, AST_NODE_KIND_BLOCK, parent);

    expect_and_consume_keyword(parser, KEYWORD_BEGIN_BLOCK);
    expect_and_consume(parser, TOKEN_KIND_PERIOD);

    while (peek_token(parser).keyword != KEYWORD_END_BLOCK) {
        ast_node *child = parse_node(parser, ret);
        slist_add(&ret->as_block.nodes, child);
    }

    expect_and_consume_keyword(parser, KEYWORD_END_BLOCK);
    expect_and_consume(parser, TOKEN_KIND_PERIOD);

    return ret;
}

static ast_node *parse_while(parser *parser, ast_node *parent) {
    // Example: "Så_længe A er_mindre_end B. [body]"

    ast_node *ret = push_ast_node(parser, AST_NODE_KIND_WHILE, parent);

    expect_and_consume_keyword(parser, KEYWORD_WHILE);
    ret->as_while.condition = parse_expr(parser);
    expect_and_consume(parser, TOKEN_KIND_PERIOD);
    ret->as_while.body = parse_node(parser, ret);
    ret->as_while.body->parent = ret;

    return ret;
}

static ast_node *parse_if(parser *parser, ast_node *parent) {
    // Example: "Hvis A er_lig_med B. [body_true] Ellers. [body_false]"

    ast_node *ret = push_ast_node(parser, AST_NODE_KIND_IF, parent);

    expect_and_consume_keyword(parser, KEYWORD_IF);
    ret->as_if.condition = parse_expr(parser);
    expect_and_consume(parser, TOKEN_KIND_PERIOD);
    ret->as_if.body_true = parse_node(parser, ret);

    expect_and_consume_keyword(parser, KEYWORD_ELSE); // @Todo If statement without else block.
    expect_and_consume(parser, TOKEN_KIND_PERIOD);
    ret->as_if.body_false = parse_node(parser, ret);

    return ret;
}

static ast_node *parse_node(parser *parser, ast_node *parent) {
    ast_node *ret = null;
    token peek = expect_and_peek(parser, TOKEN_KIND_KEYWORD);
    switch (peek.keyword) {
        case KEYWORD_DECL: ret = parse_decl(parser, parent); break;
        case KEYWORD_ASSIGN: ret = parse_assign(parser, parent); break;
        case KEYWORD_OUTPUT: ret = parse_output(parser, parent); break;
        case KEYWORD_BEGIN_BLOCK: ret = parse_block(parser, parent); break;
        case KEYWORD_WHILE: ret = parse_while(parser, parent); break;
        case KEYWORD_IF: ret = parse_if(parser, parent); break;
        default: assert(!"Unknown keyword"); break;
    }

    return ret;
}

static ast parse(tokens tokens, arena *out) {
    parser parser = { 0 };
    parser.all_tokens = tokens;
    parser.arena = out;

    token_iter_begin(&parser.token_iter, &tokens);

    ast_node *root = parse_node(&parser, null);
    ast ret = { 0 };
    ret.root = root;

    return ret;
}
