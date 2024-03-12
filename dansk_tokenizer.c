static str token_kind_as_str(token_kind token_kind) {
    switch (token_kind) {
        case TOKEN_KIND_PERIOD: return str("TOKEN_KIND_PERIOD");
        case TOKEN_KIND_KEYWORD: return str("TOKEN_KIND_KEYWORD");
        case TOKEN_KIND_IDENT: return str("TOKEN_KIND_IDENT");
        case TOKEN_KIND_LITERAL: return str("TOKEN_KIND_NUMBER");
        case TOKEN_KIND_OPERATOR: return str("TOKEN_KIND_OPERATOR");
        case TOKEN_KIND_EOF: return str("TOKEN_KIND_EOF");
        default: return str("");
    }
}

static void print_token(token token) {
    switch (token.kind) {
        case TOKEN_KIND_KEYWORD: {
            println("%:% \t % \t %var \t %",
                    token.linenumber,
                    token.column,
                    token_kind_as_str(token.kind),
                    token.str, keyword_as_str(token.keyword));
        } break;

        case TOKEN_KIND_LITERAL: {
            println("%:% \t % \t %var \t %",
                    token.linenumber,
                    token.column,
                    token_kind_as_str(token.kind), token.str, token.var_value.as_i64);
        } break;

        default: {
            println("%:% \t % \t %var",
                    token.linenumber,
                    token.column,
                    token_kind_as_str(token.kind),
                    token.str);
        } break;
    }
}

static void print_tokens(tokens tokens) {
    for_val_list(token_block, block, tokens.blocks.first) {
        for_val(token, token, block) {
            print_token(token);
        }
    }
}

static void tokenizer_exit_error(tokenizer *t, str s) {
    println("Tokenizer fejl på linje %, kolonne %", t->at_linenumber, t->at_column);
    println("    %", s);
    exit(0);
}

static bool tok_not_done(tokenizer *t) {
    bool ret = t->at < t->string.len;
    return ret;
}

static u8 tok_advance_byte(tokenizer *t) {
    assert(tok_not_done(t));
    u8 ret = t->string.v[t->at];
    t->at += 1;
    t->at_column += 1;
    return ret;
}

static u32 tok_advance(tokenizer *t) {
    assert(tok_not_done(t));
    unicode_codepoint decoded = decode_single_utf8_codepoint(substr_idx(t->string, t->at));
    t->at += decoded.len;
    t->at_column += decoded.len;
    return decoded.codepoint;
}

static u8 tok_at_byte(tokenizer *t) {
    u8 ret = t->string.v[t->at];
    return ret;
}

static u32 tok_at(tokenizer *t) {
    unicode_codepoint decoded = decode_single_utf8_codepoint(substr_idx(t->string, t->at));
    return decoded.codepoint;
}

static void tok_eat_whitespace(tokenizer *t) {
    while (tok_not_done(t) && u32_is_whitespace(tok_at_byte(t))) {
        u8 consumed = tok_advance_byte(t);
        if (consumed == '\n') {
            t->at_column = 1;
            t->at_linenumber += 1;
        }
    }
}

static bool is_danish_letter(u32 c) {
    bool ret = (c == U'Æ' || c == U'æ' ||
                c == U'Ø' || c == U'ø' ||
                c == U'Å' || c == U'å' ||
                is_ascii_letter(c) || 
                c >= 256);

    return ret;
}

static bool is_danish_word_char(u32 c) {
    bool ret = (is_danish_letter(c) || c == '_' );
    return ret;
}

static token tok_next_token(tokenizer *t) {

    token ret = { 0 };
    ret.linenumber = t->at_linenumber;
    ret.column = t->at_column;

    u32 c = tok_at(t);
    if (is_danish_word_char(c) || c >= 256) {
        i64 start_at = t->at;

        while (tok_not_done(t) && is_danish_word_char(tok_at(t))) {
            tok_advance(t);
        }

        range_ix range = range_ix(start_at, t->at);
        ret.str = substr_range(t->string, range);
        ret.kind = TOKEN_KIND_IDENT;

        keyword keyword = keyword_from_spelling(ret.str);
        if (keyword) {
            if (keyword == KEYWORD_TRUE) {
                ret.kind              = TOKEN_KIND_LITERAL;
                ret.var_value.kind    = VAR_KIND_BOOL;
                ret.var_value.as_bool = true;
            } else if (keyword == KEYWORD_FALSE) {
                ret.kind              = TOKEN_KIND_LITERAL;
                ret.var_value.kind    = VAR_KIND_BOOL;
                ret.var_value.as_bool = false;
            } else {
                ret.kind = TOKEN_KIND_KEYWORD;
                ret.keyword = keyword;
            }
        }

        operator operator = operator_from_spelling(ret.str);
        if (operator) {
            ret.kind = TOKEN_KIND_OPERATOR;
            ret.operator = operator;
        }

    } else if (is_ascii_digit(c)) {
        i64 start_at = t->at;

        while (tok_not_done(t) && (is_ascii_digit(tok_at(t)) || tok_at(t) == ',')) {
            tok_advance(t);
        }

        ret.kind = TOKEN_KIND_LITERAL;
        ret.str.v   = t->string.v + start_at;
        ret.str.len = t->at - start_at;

        arena_temp(&temp_arena) {
            str cstr = copy_str_null_terminate(&temp_arena, ret.str);
            char *start_ptr = (char *)cstr.v;
            char *end_ptr;

            bool is_decimal = false;
            for (char *d = start_ptr; *d; d++) {
                if (*d == ',') {
                    *d = '.';
                    is_decimal = true;
                }
            }

            if (is_decimal) {
                ret.var_value.as_f64 = strtof(start_ptr, &end_ptr);
                ret.var_value.kind = VAR_KIND_F64;
            } else {
                ret.var_value.as_i64 = strtoll(start_ptr, &end_ptr, 10);
                ret.var_value.kind = VAR_KIND_I64;
            }
        }

    } else if (c == '.') {
        tok_advance(t);
        ret.kind = TOKEN_KIND_PERIOD;
        ret.str = str(".");
    } else {
        tokenizer_exit_error(t, str("Uventet karakter"));
    }

    return ret;
}

static token_block *push_token_block(arena *arena, ix cap) {
    token_block *ret = push_struct(arena, token_block);
    ret->v = push_size(arena, cap * sizeof(token));
    ret->cap = cap;
    return ret;
}

static void add_token(tokens *t, token token) {
    if (!t->blocks.first) {
        t->blocks.first = t->blocks.last = push_token_block(t->arena, 128);
    }

    if (t->blocks.last->count == t->blocks.last->cap) {
        token_block *next = push_token_block(t->arena, 128);
        dlist_add(&t->blocks, next);
    }

    t->blocks.last->v[t->blocks.last->count++] = token;
}

static void token_iter_at(token_iter *iter) {
    if (iter->block) {
        assert_bounds(*iter->block, iter->idx);
        iter->at = &iter->block->v[iter->idx];
        iter->valid = true;
    } else {
        iter->valid = false;
    }
}

static void token_iter_begin(token_iter *iter, tokens *tokens) {
    zero(iter);
    iter->block = tokens->blocks.first;
    iter->idx = 0;
    token_iter_at(iter);
}

static void token_iter_next(token_iter *iter) {
    iter->idx++;
    if (iter->idx == iter->block->count) {
        iter->block = iter->block->next;
        iter->idx = 0;
    }
    token_iter_at(iter);
}

static tokens tokenize(str s, arena *out) {
    tokens tokens = { 0 };
    tokens.arena = out;

    tokenizer t = { 0 };
    t.string = s;
    t.at_linenumber = 1;
    t.at_column = 1;

    tok_eat_whitespace(&t);
    while (tok_not_done(&t)) {
        token token = tok_next_token(&t);
        add_token(&tokens, token);
        tok_eat_whitespace(&t);
    }

    return tokens;
}
