////////////////////////////////////////////////////////////////
// Enums.

typedef enum keyword {
    KEYWORD_NONE,

    KEYWORD_BEGIN_BLOCK,
    KEYWORD_END_BLOCK,
    KEYWORD_ASSIGN,
    KEYWORD_DECL,
    KEYWORD_OUTPUT,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_TRUE,
    KEYWORD_FALSE,

    KEYWORD_COUNT,
} keyword;

typedef enum operator {
    OPERATOR_NONE,

        OPERATOR_ADD,
        OPERATOR_SUB,
        OPERATOR_MUL,
        OPERATOR_DIV,
        OPERATOR_GT,
        OPERATOR_LT,
        OPERATOR_EQ,

        OPERATOR_COUNT,
} operator;

////////////////////////////////////////////////////////////////
// Enum name tables.

static readonly str keyword_names[] = {
    [KEYWORD_BEGIN_BLOCK] = static_str("KEYWORD_BEGIN_BLOCK"),
    [KEYWORD_END_BLOCK]   = static_str("KEYWORD_END_BLOCK"),
    [KEYWORD_ASSIGN]      = static_str("KEYWORD_ASSIGN"),
    [KEYWORD_DECL]        = static_str("KEYWORD_DECL"),
    [KEYWORD_OUTPUT]      = static_str("KEYWORD_OUTPUT"),
    [KEYWORD_IF]          = static_str("KEYWORD_IF"),
    [KEYWORD_ELSE]        = static_str("KEYWORD_ELSE"),
    [KEYWORD_WHILE]       = static_str("KEYWORD_WHILE"),
    [KEYWORD_TRUE]        = static_str("KEYWORD_TRUE"),  
    [KEYWORD_FALSE]       = static_str("KEYWORD_FALSE"),     
};

static readonly str operator_names[] = {
    [OPERATOR_ADD] = static_str("OPERATOR_ADD"),
    [OPERATOR_SUB] = static_str("OPERATOR_SUB"),
    [OPERATOR_MUL] = static_str("OPERATOR_MUL"),
    [OPERATOR_DIV] = static_str("OPERATOR_DIV"),
    [OPERATOR_GT]  = static_str("OPERATOR_GT"),
    [OPERATOR_LT]  = static_str("OPERATOR_LT"),
    [OPERATOR_EQ]  = static_str("OPERATOR_EQ"),
};

////////////////////////////////////////////////////////////////
// Spelling tables.

static readonly span_str keyword_spellings[] = {
    [KEYWORD_BEGIN_BLOCK] = static_strs("Goddag", "Godmorgen"),
    [KEYWORD_END_BLOCK]   = static_strs("Farvel", "Godnat"),
    [KEYWORD_ASSIGN]      = static_strs("Sæt"),
    [KEYWORD_DECL]        = static_strs("Lad"),
    [KEYWORD_OUTPUT]      = static_strs("Udskriv_i_konsol"),
    [KEYWORD_IF]          = static_strs("Hvis"),
    [KEYWORD_ELSE]        = static_strs("Ellers"),
    [KEYWORD_WHILE]       = static_strs("Så_længe"), // @Todo Multi-word keywords e.g. "Så længe".
    [KEYWORD_TRUE]        = static_strs("sand", "sandt"),
    [KEYWORD_FALSE]       = static_strs("falsk", "falskt"),
};

static readonly span_str operator_spellings[] = {
    [OPERATOR_ADD] = static_strs("plus"),
    [OPERATOR_SUB] = static_strs("minus"),
    [OPERATOR_MUL] = static_strs("gange"),
    [OPERATOR_DIV] = static_strs("divider"),
    [OPERATOR_GT]  = static_strs("er_større_end"),
    [OPERATOR_LT]  = static_strs("er_mindre_end"),
    [OPERATOR_EQ]  = static_strs("er_lig_med", "er"),
};

////////////////////////////////////////////////////////////////
// String converion functions.

static str keyword_as_str(keyword a);
static keyword keyword_from_spelling(str a);
static str operator_as_str(operator a);
static operator operator_from_spelling(str a);
