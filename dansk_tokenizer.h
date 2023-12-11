////////////////////////////////////////////////////////////////
// Token structure.

typedef enum token_kind {
    TOKEN_KIND_NONE,

    TOKEN_KIND_PERIOD,
    TOKEN_KIND_KEYWORD,
    TOKEN_KIND_IDENT,
    TOKEN_KIND_LITERAL,
    TOKEN_KIND_OPERATOR,
    TOKEN_KIND_EOF,

    TOKEN_KIND_COUNT,
} token_kind;

// @Size
typedef struct token token;
struct token {
    str str;
    token_kind kind;
    i32 column;
    i32 linenumber;

    keyword keyword;
    operator operator;
    i64 i64;
};

////////////////////////////////////////////////////////////////
// Token storage.

typedef struct token_block token_block;
struct token_block {
    token *v;
    ix count;
    ix cap;
    token_block *next, *prev;
};

typedef struct tokens tokens;
struct tokens {
    list(token_block) blocks;
    arena *arena;
};

////////////////////////////////////////////////////////////////
// Tokenizer.

typedef struct tokenizer tokenizer;
struct tokenizer {
    str string;
    i64 at;

    i32 at_column;
    i32 at_linenumber;

    token_kind current_token_kind;
    i64        current_token_start;
};

static tokens tokenize(str s, arena *out);

////////////////////////////////////////////////////////////////
// Token iterator.

typedef struct token_iter token_iter;
struct token_iter {
    token *at;
    token_block *block;
    ix idx;
    bool valid;
};

static void token_iter_begin(token_iter *iter, tokens *tokens);
static void token_iter_next(token_iter *iter);
