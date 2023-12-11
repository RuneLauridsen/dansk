////////////////////////////////////////////////////////////////
// Parser.

typedef struct parser parser;
struct parser {
    tokens all_tokens;
    token_iter token_iter;

    arena *arena;
};

static ast parse(tokens tokens, arena *out);
