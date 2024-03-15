#include "base/base.h"

arena temp_arena = { 0 };

#define tprint(...) tprint_args(argsof(__VA_ARGS__))
static str tprint_args(args args) {
    str ret = arena_push_fmt_args(&temp_arena, args);
    return ret;
}

static _Noreturn void exit_err(str msg) {
    println(msg);
    exit(1);
}

////////////////////////////////////////////////////////////////
//
//
// Tokenizer
//
//
////////////////////////////////////////////////////////////////

typedef struct dk_tokenizer dk_tokenizer;
struct dk_tokenizer {
    str src;
    u64 pos;
    u64 row;
    u64 col;
    u8 peek0;
    u8 peek1;
};

static void dk_tokenizer_eat(dk_tokenizer *t) {
    if (t->peek0 == '\n') {
        t->col = 0;
        t->row += 1;
    }

    t->pos += 1;
    t->peek0 = (t->pos + 0 < t->src.len) ? (t->src.v[t->pos + 0]) : ('\0');
    t->peek1 = (t->pos + 1 < t->src.len) ? (t->src.v[t->pos + 1]) : ('\0');
}

static void dk_tokenizer_init(dk_tokenizer *t, str src, u64 pos) {
    t->src = src;
    t->pos = pos - 1;
    dk_tokenizer_eat(t);
}

typedef enum dk_token_kind {
    DK_TOKEN_KIND_PAREN_EOF = 0,
    DK_TOKEN_KIND_PERIOD = '.',
    DK_TOKEN_KIND_PAREN_OPEN = '(',
    DK_TOKEN_KIND_PAREN_CLOSE = ')',
    DK_TOKEN_KIND_IDENT = 128,
    DK_TOKEN_KIND_NUMBER,
    DK_TOKEN_KIND_PUBLIC,
    DK_TOKEN_KIND_FUNCTION,
    DK_TOKEN_KIND_BEGIN,
    DK_TOKEN_KIND_END,
    DK_TOKEN_KIND_LET,
    DK_TOKEN_KIND_BE,
    DK_TOKEN_KIND_EN_ET,
} dk_token_kind;

static str dk_token_kind_as_str(dk_token_kind a) {
    switch (a) {
        case DK_TOKEN_KIND_PAREN_EOF:       return str("DK_TOKEN_KIND_PAREN_EOF");
        case DK_TOKEN_KIND_PERIOD:          return str("DK_TOKEN_KIND_PERIOD");
        case DK_TOKEN_KIND_PAREN_OPEN:      return str("DK_TOKEN_KIND_PAREN_OPEN");
        case DK_TOKEN_KIND_PAREN_CLOSE:     return str("DK_TOKEN_KIND_PAREN_CLOSE");
        case DK_TOKEN_KIND_IDENT:           return str("DK_TOKEN_KIND_IDENT");
        case DK_TOKEN_KIND_NUMBER:          return str("DK_TOKEN_KIND_NUMBER");
        case DK_TOKEN_KIND_PUBLIC:          return str("DK_TOKEN_KIND_PUBLIC");
        case DK_TOKEN_KIND_FUNCTION:        return str("DK_TOKEN_KIND_FUNCTION");
        case DK_TOKEN_KIND_BEGIN:           return str("DK_TOKEN_KIND_BEGIN");
        case DK_TOKEN_KIND_END:             return str("DK_TOKEN_KIND_END");
        case DK_TOKEN_KIND_LET:             return str("DK_TOKEN_KIND_LET");
        case DK_TOKEN_KIND_BE:              return str("DK_TOKEN_KIND_BE");
        case DK_TOKEN_KIND_EN_ET:           return str("DK_TOKEN_KIND_EN_ET");
        default:                            return str("INVALID");
    }
}

typedef struct dk_token_spelling dk_token_spelling;
struct dk_token_spelling {
    dk_token_kind token_kind;
    str spelling;
};

static readonly dk_token_spelling dk_token_spellings[] = {
    { DK_TOKEN_KIND_PERIOD,         STR("."),           },
    { DK_TOKEN_KIND_PAREN_OPEN,     STR("("),           },
    { DK_TOKEN_KIND_PAREN_CLOSE,    STR(")"),           },
    { DK_TOKEN_KIND_PUBLIC,         STR("offentlig"),   },
    { DK_TOKEN_KIND_FUNCTION,       STR("funktion"),    },
    { DK_TOKEN_KIND_BEGIN,          STR("goddag"),      },
    { DK_TOKEN_KIND_END,            STR("farvel"),      },
    { DK_TOKEN_KIND_LET,            STR("lad"),         },
    { DK_TOKEN_KIND_EN_ET,          STR("en"),          },
    { DK_TOKEN_KIND_EN_ET,          STR("et"),          },
    { DK_TOKEN_KIND_BE,             STR("være"),        },
};

typedef struct dk_token dk_token;
struct dk_token {
    u64 pos;
    u64 row;
    u64 col;
    str text;
    dk_token_kind kind;
};

static dk_token_kind dk_token_kind_from_str(str s) {
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

    //- Eat whitespace
    while (u8_get_char_flags(t->peek0) & CHAR_FLAG_WHITESPACE) {
        dk_tokenizer_eat(t);
    }

    dk_token_kind kind = 0;
    char_flags flags = u8_get_char_flags(t->peek0);
    u64 begin_pos = t->pos;
    u64 begin_row = t->row;
    u64 begin_col = t->col;

    //- Punctuation token
    if (flags & CHAR_FLAG_PUNCT) {
        dk_tokenizer_eat(t); // TODO(rune): Multi-character operators like +=

        str s = substr_len(t->src, begin_pos, t->pos - begin_pos);
        kind = dk_token_kind_from_str(s);
        if (kind == 0) {
            exit_err(tprint("Invalid operator at %:%", t->row + 1, t->col + 1));
        }
    }

    //- Word/keyword token
    else if (flags & (CHAR_FLAG_LETTER|CHAR_FLAG_NON_ASCII)) {
        while (u8_is_letter(t->peek0) || u8_is_digit(t->peek0) || t->peek0 == '_' || t->peek0 >= 128) {
            dk_tokenizer_eat(t);
            flags = u8_get_char_flags(t->peek0);
        }

        str s = substr_len(t->src, begin_pos, t->pos - begin_pos);
        kind = dk_token_kind_from_str(s);
        if (kind == 0) {
            kind = DK_TOKEN_KIND_IDENT;
        }
    }

    else if (flags & (CHAR_FLAG_DIGIT)) {
        kind = DK_TOKEN_KIND_NUMBER;
        while (u8_is_digit(t->peek0)) {
            dk_tokenizer_eat(t);
        }
    }

    u64 end_pos = t->pos;
    token->kind = kind;
    token->text = substr_len(t->src, begin_pos, end_pos - begin_pos);
    token->pos = begin_pos;
    token->row = begin_row;
    token->col = begin_col;

    return kind != 0;
}

////////////////////////////////////////////////////////////////
//
//
// Abstract syntax tree
//
//
////////////////////////////////////////////////////////////////

typedef struct dk_ast_expr dk_ast_expr;
struct dk_ast_expr {
    int todo;
};

typedef enum dk_ast_stmt_kind {
    DK_AST_STMT_KIND_NONE,
    DK_AST_STMT_KIND_DECL,
    DK_AST_STMT_KIND_EXPR
} dk_ast_stmt_kind;

typedef struct dk_ast_stmt_decl dk_ast_stmt_decl;
struct dk_ast_stmt_decl {
    str ident;
    str type;
    dk_ast_expr *expr; // NOTE(rune): Can be null
};

typedef struct dk_ast_stmt_expr dk_ast_stmt_expr;
struct dk_ast_stmt_expr {
    dk_ast_expr *expr;
};

typedef struct dk_ast_stmt dk_ast_stmt;
struct dk_ast_stmt {
    dk_ast_stmt_kind kind;
    union {
        dk_ast_stmt_decl as_decl;
        dk_ast_stmt_expr as_expr;
    };

    dk_ast_stmt *next;
};

typedef struct dk_ast_func dk_ast_func;
struct dk_ast_func {
    str ident;
    list(dk_ast_stmt) stmts;
};

typedef enum dk_ast_symbol_kind {
    DK_AST_SYMBOL_KIND_NONE,
    DK_AST_SYMBOL_KIND_FUNC,
} dk_ast_symbol_kind;

typedef struct dk_ast_symbol dk_ast_symbol;
struct dk_ast_symbol {
    dk_ast_symbol_kind kind;
    dk_ast_func as_func;

    dk_ast_symbol *next;
};

typedef struct dk_ast dk_ast;
struct dk_ast {
    list(dk_ast_symbol) symbols;
};

////////////////////////////////////////////////////////////////
//
//
// Abstract syntax tree print
//
//
////////////////////////////////////////////////////////////////

static void dk_print_level(u64 level) {
    print(ANSICONSOLE_FG_GRAY);
    for_n (u64, i, level) {
        print("->  ");
    }
    print(ANSICONSOLE_FG_DEFAULT);
}

static void dk_print_ast_stmt(dk_ast_stmt *stmt, u64 level) {
    dk_print_level(level);

    switch (stmt->kind) {
        case DK_AST_STMT_KIND_DECL: {
            dk_ast_stmt_decl *decl = &stmt->as_decl;
            println("stmt/decl $% type $%", decl->ident, decl->type);
            if (decl->expr) {
                assert(false && "Not implemented.");
            }
        } break;

        case DK_AST_STMT_KIND_EXPR: {
            assert(false && "Not implemented.");
        } break;

        default: {
            assert(false && "Invalid stmt kind.");
        } break;
    }
}

static void dk_print_ast_symbol(dk_ast_symbol *symbol, u64 level) {
    dk_print_level(level);

    switch (symbol->kind) {
        case DK_AST_SYMBOL_KIND_FUNC: {
            dk_ast_func *func = &symbol->as_func;
            println("symbol/func $%", func->ident);
            for_list (dk_ast_stmt, stmt, func->stmts) {
                dk_print_ast_stmt(stmt, level + 1);
            }
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

////////////////////////////////////////////////////////////////
//
//
// Parser
//
//
////////////////////////////////////////////////////////////////

static arena dk_ast_arena = { 0 };

typedef struct dk_parser dk_parser;
struct dk_parser {
    dk_tokenizer tokenizer;
    dk_token peek;
};

static dk_token dk_eat_token(dk_parser *p) {
    dk_token consumed = { 0 };

    do {
        consumed = p->peek;
        dk_next_token(&p->tokenizer, &p->peek);
    } while (consumed.kind == DK_TOKEN_KIND_EN_ET); // NOTE(rune): Skip all 'en' and 'et.

    return consumed;
}

static dk_token dk_eat_token_kind(dk_parser *p, dk_token_kind kind) {
    dk_token consumed = dk_eat_token(p);
    if (consumed.kind != kind) {
        exit_err(tprint("Unexpected token at %:%", consumed.row, consumed.col)); // TODO(rune): Better error message.
    }
    return consumed;
}

static void dk_parser_init(dk_parser *p, str s) {
    dk_tokenizer_init(&p->tokenizer, s, 0);
    dk_next_token(&p->tokenizer, &p->peek);
}

static dk_ast_expr *dk_parse_expr(dk_parser *p) {
    dk_ast_expr *expr = arena_push_struct(&dk_ast_arena, dk_ast_expr);
    assert(false && "Not implemented.");
    return expr;
}

static dk_ast_stmt *dk_parse_stmt(dk_parser *p) {
    dk_ast_stmt *stmt = arena_push_struct(&dk_ast_arena, dk_ast_stmt);

    //- Declaration statement.
    if (p->peek.kind == DK_TOKEN_KIND_LET) {
        dk_eat_token_kind(p, DK_TOKEN_KIND_LET);

        dk_token ident_token = dk_eat_token_kind(p, DK_TOKEN_KIND_IDENT);
        dk_eat_token_kind(p, DK_TOKEN_KIND_BE);
        dk_token type_token = dk_eat_token_kind(p, DK_TOKEN_KIND_IDENT);

        //- Optional initial value expression.
        dk_ast_expr *expr = null;
        if (p->peek.kind != DK_TOKEN_KIND_PERIOD) {
            expr = dk_parse_expr(p);
        }

        dk_eat_token_kind(p, DK_TOKEN_KIND_PERIOD);

        stmt->kind          = DK_AST_STMT_KIND_DECL;
        stmt->as_decl.ident = ident_token.text;
        stmt->as_decl.type  = type_token.text;
        stmt->as_decl.expr  = expr;
    }

    //- Expression statement. 
    else {
        dk_ast_expr *expr = dk_parse_expr(p);
        dk_eat_token_kind(p, DK_TOKEN_KIND_PERIOD);

        stmt->kind = DK_AST_STMT_KIND_EXPR;
        stmt->as_expr.expr = expr;
    }

    return stmt;
}

static dk_ast_func dk_parse_func(dk_parser *p) {
    dk_ast_func func = { 0 };

    //- Signature
    dk_eat_token_kind(p, DK_TOKEN_KIND_FUNCTION);
    dk_token ident_token = dk_eat_token_kind(p, DK_TOKEN_KIND_IDENT);
    func.ident = ident_token.text;

    dk_eat_token_kind(p, DK_TOKEN_KIND_PAREN_OPEN); // TODO(rune): Function argument.
    dk_eat_token_kind(p, DK_TOKEN_KIND_PAREN_CLOSE);

    //- Body
    dk_eat_token_kind(p, DK_TOKEN_KIND_BEGIN);
    while (p->peek.kind != DK_TOKEN_KIND_END) {
        dk_ast_stmt *stmt = dk_parse_stmt(p);
        slist_add(&func.stmts, stmt);
    }
    dk_eat_token_kind(p, DK_TOKEN_KIND_END);

    return func;

}

static dk_ast_symbol *dk_parse_symbol(dk_parser *p) {
    dk_ast_symbol *symbol = arena_push_struct(&dk_ast_arena, dk_ast_symbol);

    //- Visibilty
    bool is_public = false;
    switch (p->peek.kind) {
        case DK_TOKEN_KIND_PUBLIC: {
            dk_eat_token(p);
            is_public = true;
        } break;
    }

    switch (p->peek.kind) {
        case DK_TOKEN_KIND_FUNCTION: {
            symbol->kind = DK_AST_SYMBOL_KIND_FUNC;
            symbol->as_func = dk_parse_func(p);
        } break;

        default: {
            assert(false && "Not implemented");
        } break;
    }

    return symbol;
}

static dk_ast dk_parse(str s) {
    dk_ast ast = { 0 };

    dk_parser p = { 0 };
    dk_parser_init(&p, s);

    dk_ast_symbol *symbol = dk_parse_symbol(&p);
    slist_add(&ast.symbols, symbol);

    return ast;
}

////////////////////////////////////////////////////////////////
//
//
// Driver
//
//
////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
    if (argc < 2) {
        println("Usage: % <program.dk>", argv[0]);
        exit(0);
    }

    SetConsoleOutputCP(65001); // utf8

    arena arena = { 0 };

    str file_name = str_from_cstr(argv[1]);
    str file_data = os_read_entire_file(file_name, &arena);

#if 0
    dk_tokenizer t = { 0 };
    dk_tokenizer_init(&t, file_data, 0);

    dk_token token = { 0 };
    while (dk_next_token(&t, &token)) {
        print(dk_token_kind_as_str(token.kind));
        print("\t");
        print(token.text);
        print("\n");
}
#endif

    dk_ast ast = dk_parse(file_data);
    dk_print_ast(ast);
}
