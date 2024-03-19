////////////////////////////////////////////////////////////////
//
//
// Configuration
//
//
////////////////////////////////////////////////////////////////

#define DK_DEBUG_PRINT_LEX      0
#define DK_DEBUG_PRINT_PARSE    0
#define DK_DEBUG_PRINT_EMIT     0

////////////////////////////////////////////////////////////////
//
//
// Errors
//
//
////////////////////////////////////////////////////////////////

static arena temp_arena = { 0 };

typedef struct dk_err dk_err;
struct dk_err {
    str msg;
    dk_err *next;
};

typedef struct dk_err_list dk_err_list;
struct dk_err_list {
    dk_err *first;
    dk_err *last;
    u64 count;
};

typedef struct dk_err_sink dk_err_sink;
struct dk_err_sink {
    dk_err_list err_list;
};

#define                 dk_tprint(...) dk_tprint_args(argsof(__VA_ARGS__))
static str              dk_tprint_args(args args);
#if 0
static _Noreturn void   dk_exit_err(str msg);
#endif

static void dk_report_err(dk_err_sink *sink, str msg);
static str dk_str_from_err_sink(dk_err_sink *sink, arena *arena);

////////////////////////////////////////////////////////////////
//
//
// Tokenization
//
//
////////////////////////////////////////////////////////////////

typedef enum dk_token_kind {
    DK_TOKEN_KIND_EOF,

    //- Operators
    DK_TOKEN_KIND_ADD,
    DK_TOKEN_KIND_SUB,
    DK_TOKEN_KIND_MUL,
    DK_TOKEN_KIND_DIV,
    DK_TOKEN_KIND_AND,
    DK_TOKEN_KIND_OR,

    //- Punctuation
    DK_TOKEN_KIND_DOT,
    DK_TOKEN_KIND_COMMA,
    DK_TOKEN_KIND_PAREN_OPEN,
    DK_TOKEN_KIND_PAREN_CLOSE,

    //- Generic tokens
    DK_TOKEN_KIND_IDENT,
    DK_TOKEN_KIND_LITERAL_INT,
    DK_TOKEN_KIND_LITERAL_FLOAT,

    //- Keywords
    DK_TOKEN_KIND_ASSIGN,
    DK_TOKEN_KIND_PUBLIC,
    DK_TOKEN_KIND_PROC,
    DK_TOKEN_KIND_BEGIN,
    DK_TOKEN_KIND_END,
    DK_TOKEN_KIND_LET,
    DK_TOKEN_KIND_BE,
    DK_TOKEN_KIND_TO,
    DK_TOKEN_KIND_AS,
    DK_TOKEN_KIND_CALL,
    DK_TOKEN_KIND_EN_ET,
    DK_TOKEN_KIND_RETURN,
    DK_TOKEN_KIND_COMMENT,
    DK_TOKEN_KIND_FALSE,
    DK_TOKEN_KIND_TRUE,
    DK_TOKEN_KIND_IF,
    DK_TOKEN_KIND_ELSE,
    DK_TOKEN_KIND_KEYWORD,
    DK_TOKEN_KIND_ARGUMENT,

    DK_TOKEN_KIND_COUNT,
} dk_token_kind;

typedef struct dk_token_spelling dk_token_spelling;
struct dk_token_spelling {
    dk_token_kind token_kind;
    str spelling;
};

static readonly dk_token_spelling dk_token_spellings[] = {
    { DK_TOKEN_KIND_DOT,            STR("."),            },
    { DK_TOKEN_KIND_COMMA,          STR(","),            },
    { DK_TOKEN_KIND_PAREN_OPEN,     STR("("),            },
    { DK_TOKEN_KIND_PAREN_CLOSE,    STR(")"),            },
                                                         
    { DK_TOKEN_KIND_PUBLIC,         STR("offentlig"),    },
    { DK_TOKEN_KIND_PROC,           STR("funktion"),     },
                                                         
    { DK_TOKEN_KIND_BEGIN,          STR("goddag"),       },
    { DK_TOKEN_KIND_END,            STR("farvel"),       },
    { DK_TOKEN_KIND_BEGIN,          STR("godmorgen"),    },
    { DK_TOKEN_KIND_END,            STR("godnat"),       },
                                                         
    { DK_TOKEN_KIND_LET,            STR("lad"),          },
    { DK_TOKEN_KIND_EN_ET,          STR("en"),           },
    { DK_TOKEN_KIND_EN_ET,          STR("et"),           },
    { DK_TOKEN_KIND_BE,             STR("være"),         },
    { DK_TOKEN_KIND_TO,             STR("til"),          },
    { DK_TOKEN_KIND_AS,             STR("som"),           },
    { DK_TOKEN_KIND_ASSIGN,         STR("sæt"),          },
                                                         
    { DK_TOKEN_KIND_CALL,           STR("kald"),         },
    { DK_TOKEN_KIND_RETURN,         STR("tilbagegiv"),   },
    { DK_TOKEN_KIND_RETURN,         STR("tilbagegiver"), },
    { DK_TOKEN_KIND_IF,             STR("hvis"),         },
    { DK_TOKEN_KIND_ELSE,           STR("ellers"),       },
    { DK_TOKEN_KIND_ARGUMENT,       STR("tager"),        },
                                                         
    { DK_TOKEN_KIND_ADD,            STR("plus"),         },
    { DK_TOKEN_KIND_SUB,            STR("minus"),        },
    { DK_TOKEN_KIND_MUL,            STR("gange"),        },
    { DK_TOKEN_KIND_AND,            STR("og"),           },
    { DK_TOKEN_KIND_AND,            STR("samt"),         },
    { DK_TOKEN_KIND_OR,             STR("eller"),        },
                                                         
    { DK_TOKEN_KIND_FALSE,          STR("falsk"),        },
    { DK_TOKEN_KIND_FALSE,          STR("falskt"),       },
    { DK_TOKEN_KIND_TRUE,           STR("sand"),         },
    { DK_TOKEN_KIND_TRUE,           STR("sandt"),        },
                                                         
    { DK_TOKEN_KIND_COMMENT,        STR("bemærk"),       },
};

typedef struct dk_token dk_token;
struct dk_token {
    u64 pos;
    u64 row;
    u64 col;
    union { i64 i64_; f64 f64_; };
    str text;
    dk_token_kind kind;
};

typedef struct dk_tokenizer dk_tokenizer;
struct dk_tokenizer {
    str src;
    u64 pos;
    u64 row;
    u64 col;
    u8 peek0;
    u8 peek1;
};

static str           dk_token_kind_as_str(dk_token_kind a);
static dk_token_kind dk_token_kind_from_spelling(str s);

static void          dk_tokenizer_eat(dk_tokenizer *t);
static void          dk_tokenizer_init(dk_tokenizer *t, str src, u64 pos);

static bool          dk_next_token(dk_tokenizer *t, dk_token *token);

////////////////////////////////////////////////////////////////
//
//
// Abstract syntax tree
//
//
////////////////////////////////////////////////////////////////

typedef enum dk_ast_literal_kind {
    DK_AST_LITERAL_KIND_NONE,
    DK_AST_LITERAL_KIND_INT,
    DK_AST_LITERAL_KIND_FLOAT,
    DK_AST_LITERAL_KIND_BOOL,

    DK_AST_LITERAL_KIND_COUNT,
} dk_ast_literal_kind;

typedef struct dk_ast_literal dk_ast_literal;
struct dk_ast_literal {
    dk_ast_literal_kind kind;
    union {
        i64 i64_;
        f64 f64_;
        bool boolean;
    };
};

typedef enum dk_ast_expr_kind {
    DK_AST_EXPR_KIND_NONE,
    DK_AST_EXPR_KIND_CALL,
    DK_AST_EXPR_KIND_UNARY,
    DK_AST_EXPR_KIND_BINARY,
    DK_AST_EXPR_KIND_LITERAL,
    DK_AST_EXPR_KIND_IDENT,

    DK_AST_EXPR_KIND_COUNT,
} dk_ast_expr_kind;

typedef struct dk_ast_expr dk_ast_expr;

typedef struct dk_ast_expr_arg dk_ast_expr_arg;
struct dk_ast_expr_arg {
    dk_ast_expr *expr;
    dk_ast_expr_arg *next;
};

typedef struct dk_ast_expr_arg_list dk_ast_expr_arg_list;
struct dk_ast_expr_arg_list {
    dk_ast_expr_arg *first;
    dk_ast_expr_arg *last;
    u32 count;
};

typedef struct dk_ast_expr_unary dk_ast_expr_unary;
struct dk_ast_expr_unary {
    dk_token_kind operator;
    dk_ast_expr *operand;
};

typedef struct dk_ast_expr_binary dk_ast_expr_binary;
struct dk_ast_expr_binary {
    dk_token_kind operator;
    dk_ast_expr *lhs;
    dk_ast_expr *rhs;
};

typedef struct dk_ast_expr_call dk_ast_expr_call;
struct dk_ast_expr_call {
    str ident;
    dk_ast_expr_arg_list args;
};

typedef struct dk_ast_expr dk_ast_expr;
struct dk_ast_expr {
    dk_ast_expr_kind kind;
    union {
        dk_ast_expr_call call;
        dk_ast_expr_unary unary;
        dk_ast_expr_binary binary;
        dk_ast_literal literal;
        str ident;
    };
    struct dk_symbol *sym;
};

typedef enum dk_ast_stmt_kind {
    DK_AST_STMT_KIND_NONE,
    DK_AST_STMT_KIND_DECL,
    DK_AST_STMT_KIND_EXPR,
    DK_AST_STMT_KIND_ASSIGN,
    DK_AST_STMT_KIND_RETURN,
    DK_AST_STMT_KIND_IF,

    DK_AST_STMT_KIND_COUNT,
} dk_ast_stmt_kind;

typedef struct dk_ast_decl dk_ast_decl;
struct dk_ast_decl {
    str ident;
    str type_name;
    dk_ast_expr *expr; // NOTE(rune): Can be null
};

typedef struct dk_ast_assign dk_ast_assign;
struct dk_ast_assign {
    dk_ast_expr *left;
    dk_ast_expr *right;
};

typedef struct dk_ast_return dk_ast_return;
struct dk_ast_return {
    dk_ast_expr *expr; // NOTE(rune): Can be null
};

typedef struct dk_ast_stmts dk_ast_stmts;
struct dk_ast_stmts {
    struct dk_ast_stmt *first;
    struct dk_ast_stmt *last;
};

typedef struct dk_ast_if dk_ast_if;
struct dk_ast_if {
    dk_ast_expr *expr;
    dk_ast_stmts then;
    dk_ast_stmts else_;
};

typedef struct dk_ast_stmt dk_ast_stmt;
struct dk_ast_stmt {
    dk_ast_stmt_kind kind;
    union {
        dk_ast_decl decl;
        dk_ast_expr *expr;
        dk_ast_assign assign;
        dk_ast_return return_;
        dk_ast_if if_;
    };
    dk_ast_stmt *next;
};

typedef struct dk_ast_proc_arg dk_ast_proc_arg;
struct dk_ast_proc_arg {
    str name;
    str type_name;
    struct dk_symbol *symbol;
    dk_ast_proc_arg *next;
};

typedef struct dk_ast_proc_args dk_ast_proc_args;
struct dk_ast_proc_args {
    dk_ast_proc_arg *first;
    dk_ast_proc_arg *last;
    u64 count;
};

typedef struct dk_ast_proc dk_ast_proc;
struct dk_ast_proc {
    str ident;
    str return_type_name;
    struct dk_symbol *type;
    dk_ast_stmts stmts;
    dk_ast_proc_args args;
};

typedef enum dk_ast_visibilty {
    DK_AST_VISIBILTY_NONE,
    DK_AST_VISIBILTY_PRIVATE,
    DK_AST_VISIBILTY_PUBLIC,

    DK_AST_VISIBILTY_COUNT,
} dk_ast_visibilty;

typedef enum dk_ast_symbol_kind {
    DK_AST_SYMBOL_KIND_NONE,
    DK_AST_SYMBOL_KIND_PROC,

    DK_AST_SYMBOL_KIND_COUNT,
} dk_ast_symbol_kind;

typedef struct dk_ast_symbol dk_ast_symbol;
struct dk_ast_symbol {
    dk_ast_symbol_kind kind;
    union {
        dk_ast_proc proc;
    };

    dk_ast_symbol *next;
};

typedef struct dk_ast dk_ast;
struct dk_ast {
    list(dk_ast_symbol) symbols;
};

////////////////////////////////////////////////////////////////
//
//
// Operators
//
//
////////////////////////////////////////////////////////////////

typedef struct dk_operator_info dk_operator_info;
struct dk_operator_info {
    dk_ast_expr_kind expr_kind;
    u8 precedence;
};

static readonly dk_operator_info dk_operator_infos_prefix[DK_TOKEN_KIND_COUNT] = {
    [DK_TOKEN_KIND_ASSIGN] =        { DK_AST_EXPR_KIND_BINARY,        1, },
    [DK_TOKEN_KIND_IDENT]         = { DK_AST_EXPR_KIND_IDENT,         1, },
    [DK_TOKEN_KIND_LITERAL_INT]   = { DK_AST_EXPR_KIND_LITERAL,       1, },
    [DK_TOKEN_KIND_LITERAL_FLOAT] = { DK_AST_EXPR_KIND_LITERAL,       1, },
    [DK_TOKEN_KIND_FALSE]         = { DK_AST_EXPR_KIND_LITERAL,       1, },
    [DK_TOKEN_KIND_TRUE]          = { DK_AST_EXPR_KIND_LITERAL,       1, },
    [DK_TOKEN_KIND_PAREN_OPEN]    = { 0,                              1, },
};

static readonly dk_operator_info dk_operator_infos[DK_TOKEN_KIND_COUNT] = {
    [DK_TOKEN_KIND_OR] =   { DK_AST_EXPR_KIND_BINARY, 2, },
    [DK_TOKEN_KIND_AND]  = { DK_AST_EXPR_KIND_BINARY, 3, },
    [DK_TOKEN_KIND_ADD]  = { DK_AST_EXPR_KIND_BINARY, 4, },
    [DK_TOKEN_KIND_SUB]  = { DK_AST_EXPR_KIND_BINARY, 4, },
    [DK_TOKEN_KIND_MUL]  = { DK_AST_EXPR_KIND_BINARY, 5, },
    [DK_TOKEN_KIND_DIV]  = { DK_AST_EXPR_KIND_BINARY, 5, },
    [DK_TOKEN_KIND_CALL] = { 0,                       10, },
};

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

// High level api.
static dk_ast dk_ast_from_str(str s);

// Consume token helpers.
static dk_token dk_eat_token(dk_parser *p);
static dk_token dk_eat_token_kind(dk_parser *p, dk_token_kind kind);
static bool     dk_eat_token_kind_maybe(dk_parser *p, dk_token_kind kind);

// Parse functions.
static dk_ast_literal   dk_parse_literal(dk_parser *p);
static dk_ast_expr *    dk_parse_expr(dk_parser *p, u64 precedence);
static dk_ast_stmt *    dk_parse_stmt(dk_parser *p);
static dk_ast_stmts     dk_parse_stmts(dk_parser *p);
static dk_ast_proc      dk_parse_proc(dk_parser *p);
static dk_ast_visibilty dk_parse_visibility(dk_parser *p);
static dk_ast_symbol *  dk_parse_symbol(dk_parser *p);

// Initialization
static void dk_parser_init(dk_parser *p, str s);

////////////////////////////////////////////////////////////////
//
//
// Symbol table
//
//
////////////////////////////////////////////////////////////////

typedef enum dk_symbol_kind {
    DK_SYMBOL_KIND_NONE,
    DK_SYMBOL_KIND_PROC,
    DK_SYMBOL_KIND_TYPE,
    DK_SYMBOL_KIND_LOCAL,

    DK_SYMBOL_KIND_COUNT,
} dk_symbol_kind;

typedef struct dk_symbol dk_symbol;
struct dk_symbol {
    dk_symbol_kind kind;

    str name;
    dk_ast_symbol *ast;

    bool builtin;
    u32 id;
    u64 size;

    enum dk_type_kind type_kind;
    dk_symbol *type;
};

typedef struct dk_symbol_table dk_symbol_table;
struct dk_symbol_table {
    dk_symbol *slots; // TODO(rune): De-array-ify, and hash-table-ify
    u64 slot_count;
    u64 slot_capacity;
};

static dk_symbol *dk_symbol_table_add(dk_symbol_table *table, str name);

////////////////////////////////////////////////////////////////
//
//
// Type check and symbol resolution
//
//
////////////////////////////////////////////////////////////////

typedef enum dk_type_kind {
    DK_TYPE_KIND_NONE,
    DK_TYPE_KIND_VOID,
    DK_TYPE_KIND_BOOL,
    DK_TYPE_KIND_I8,
    DK_TYPE_KIND_I16,
    DK_TYPE_KIND_I32,
    DK_TYPE_KIND_I64,
    DK_TYPE_KIND_U8,
    DK_TYPE_KIND_U16,
    DK_TYPE_KIND_U32,
    DK_TYPE_KIND_U64,
    DK_TYPE_KIND_F32,
    DK_TYPE_KIND_F64,

    DK_TYPE_KIND_STRUCT,

    DK_TYPE_KIND_COUNT,
} dk_type_kind;

typedef struct dk_type_kind_info dk_type_kind_info;
struct dk_type_kind_info {
    str spelling;
    u32 size;
};

static readonly dk_type_kind_info dk_type_kind_infos[DK_TYPE_KIND_COUNT] = {
    [DK_TYPE_KIND_NONE] = { STR("NONE"), },
    [DK_TYPE_KIND_VOID]     = { STR("void"), },
    [DK_TYPE_KIND_BOOL]     = { STR("bool"), },
    [DK_TYPE_KIND_I8]       = { STR("i8"),   },
    [DK_TYPE_KIND_I16]      = { STR("i16"),  },
    [DK_TYPE_KIND_I32]      = { STR("i32"),  },
    [DK_TYPE_KIND_I64]      = { STR("i64"),  },
    [DK_TYPE_KIND_U8]       = { STR("u8"),   },
    [DK_TYPE_KIND_U16]      = { STR("u16"),  },
    [DK_TYPE_KIND_U32]      = { STR("u32"),  },
    [DK_TYPE_KIND_U64]      = { STR("u64"),  },
    [DK_TYPE_KIND_F32]      = { STR("f32"),  },
    [DK_TYPE_KIND_F64]      = { STR("f64"),  },
    [DK_TYPE_KIND_STRUCT]   = { STR("struct"), },
};

typedef struct dk_type dk_type;
struct dk_type {
    dk_type_kind kind;
    str name;
    u64 size;
};

static readonly dk_type dk_base_types[] = {
    { DK_TYPE_KIND_VOID,    STR("tomrum"),      0,  },
    { DK_TYPE_KIND_BOOL,    STR("påstand"),     1,  },
    //{ DK_TYPE_KIND_I8,      STR(""),            1,  },
    //{ DK_TYPE_KIND_I16,     STR(""),            2,  },
    //{ DK_TYPE_KIND_I32,     STR(""),            4,  },
    { DK_TYPE_KIND_I64,     STR("heltal"),      8,  },
    //{ DK_TYPE_KIND_U8,      STR(""),            1,  },
    //{ DK_TYPE_KIND_U16,     STR(""),            2,  },
    //{ DK_TYPE_KIND_U32,     STR(""),            4,  },
    //{ DK_TYPE_KIND_U64,     STR(""),            8,  },
    //{ DK_TYPE_KIND_F32,     STR(""),            4,  },
    { DK_TYPE_KIND_F64,     STR("flyder"),      8,  },
};

typedef struct dk_checker dk_checker;
struct dk_checker {
    dk_symbol_table global_symbols;

    dk_symbol_table local_symbols;

    u32 proc_id_counter;
    u32 type_id_counter;
};

//- High level api.
static dk_symbol_table      dk_check_ast(dk_ast ast);

//- Checker functions.
static dk_symbol *          dk_check_symbol(dk_checker *checker, str name);
static dk_symbol *          dk_check_expr(dk_checker *checker, dk_ast_expr *expr);
static void                 dk_check_proc(dk_checker *checker, dk_symbol *symbol);

////////////////////////////////////////////////////////////////
//
//
// Bytecode
//
//
////////////////////////////////////////////////////////////////

typedef enum dk_bc_opcode {
    DK_BC_OPCODE_NOP,

    DK_BC_OPCODE_LDI,
    DK_BC_OPCODE_LDL,
    DK_BC_OPCODE_STL,
    DK_BC_OPCODE_POP,

    DK_BC_OPCODE_ADD,
    DK_BC_OPCODE_SUB,
    DK_BC_OPCODE_UMUL,
    DK_BC_OPCODE_IMUL,
    DK_BC_OPCODE_UDIV,
    DK_BC_OPCODE_IDIV,

    DK_BC_OPCODE_FADD,
    DK_BC_OPCODE_FSUB,
    DK_BC_OPCODE_FMUL,
    DK_BC_OPCODE_FDIV,

    DK_BC_OPCODE_AND,
    DK_BC_OPCODE_OR,
    DK_BC_OPCODE_NOT,

    DK_BC_OPCODE_CALL,
    DK_BC_OPCODE_RET,
    DK_BC_OPCODE_BR,

    DK_BC_OPCODE_COUNT,
} dk_bc_opcode;

typedef enum dk_bc_operand_kind {
    DK_BC_OPERAND_KIND_NONE,
    DK_BC_OPERAND_KIND_IMM,
    DK_BC_OPERAND_KIND_LOC,
    DK_BC_OPERAND_KIND_SYM,
    DK_BC_OPERAND_KIND_POS,

    DK_BC_OPERAND_KIND_COUNT,
} dk_bc_operand_kind;

typedef union dk_bc_inst_prefix dk_bc_inst_prefix;
union dk_bc_inst_prefix {
    struct { u8 opcode : 6, operand_size : 2; };
    struct { u8 u8; };
};

typedef struct dk_bc_inst dk_bc_inst;
struct dk_bc_inst {
    u8 opcode;
    u64 operand;
};

typedef struct dk_bc_opcode_info dk_bc_opcode_info;
struct dk_bc_opcode_info {
    str name;
    dk_bc_operand_kind operand_kind;
};

static readonly dk_bc_opcode_info dk_bc_opcode_infos[DK_BC_OPCODE_COUNT] = {
#if 1 // English mnemonics
    [DK_BC_OPCODE_NOP] =   { STR("nop"),                                 },
    [DK_BC_OPCODE_LDI]   = { STR("ldi"),      DK_BC_OPERAND_KIND_IMM     },
    [DK_BC_OPCODE_LDL]   = { STR("ldl"),      DK_BC_OPERAND_KIND_LOC     },
    [DK_BC_OPCODE_POP]   = { STR("pop"),                                 },
    [DK_BC_OPCODE_STL]   = { STR("stl"),      DK_BC_OPERAND_KIND_LOC     },

    [DK_BC_OPCODE_ADD]   = { STR("add"),                                 },
    [DK_BC_OPCODE_SUB]   = { STR("sub"),                                 },
    [DK_BC_OPCODE_UMUL]  = { STR("umul"),                                },
    [DK_BC_OPCODE_IMUL]  = { STR("imul"),                                },
    [DK_BC_OPCODE_UDIV]  = { STR("udiv"),                                },
    [DK_BC_OPCODE_IDIV]  = { STR("idiv"),                                },

    [DK_BC_OPCODE_FADD]  = { STR("fadd"),                                },
    [DK_BC_OPCODE_FSUB]  = { STR("fsub"),                                },
    [DK_BC_OPCODE_FMUL]  = { STR("fmul"),                                },
    [DK_BC_OPCODE_FDIV]  = { STR("fdiv"),                                },

    [DK_BC_OPCODE_AND]   = { STR("and"),                                 },
    [DK_BC_OPCODE_OR]    = { STR("or"),                                  },
    [DK_BC_OPCODE_NOT]   = { STR("not"),                                 },

    [DK_BC_OPCODE_CALL]  = { STR("call"),     DK_BC_OPERAND_KIND_SYM     },
    [DK_BC_OPCODE_RET]   = { STR("ret"),                                 },
    [DK_BC_OPCODE_BR]    = { STR("br"),       DK_BC_OPERAND_KIND_POS     },
#else
    [DK_BC_OPCODE_NOP] =   { STR("nop"),                                 },
    [DK_BC_OPCODE_LDI]   = { STR("ilu"),      DK_BC_OPERAND_KIND_IMM     }, // Indlæs umiddelbar
    [DK_BC_OPCODE_LDL]   = { STR("ill"),      DK_BC_OPERAND_KIND_LOC     }, // Indlæs lokal
    [DK_BC_OPCODE_POP]   = { STR("tag"),                                 }, 
    [DK_BC_OPCODE_STL]   = { STR("gel"),      DK_BC_OPERAND_KIND_LOC     }, // Gem lokal

    [DK_BC_OPCODE_ADD]   = { STR("plus"),                                 },
    [DK_BC_OPCODE_SUB]   = { STR("minus"),                                 },
    [DK_BC_OPCODE_UMUL]  = { STR("ugange"),                                },
    [DK_BC_OPCODE_IMUL]  = { STR("igange"),                                },
    [DK_BC_OPCODE_UDIV]  = { STR("udel"),                                },
    [DK_BC_OPCODE_IDIV]  = { STR("idel"),                                },

    [DK_BC_OPCODE_FADD]  = { STR("fplus"),                                },
    [DK_BC_OPCODE_FSUB]  = { STR("fminus"),                                },
    [DK_BC_OPCODE_FMUL]  = { STR("fgange"),                                },
    [DK_BC_OPCODE_FDIV]  = { STR("fdel"),                                },

    [DK_BC_OPCODE_AND]   = { STR("or"),                                 },
    [DK_BC_OPCODE_OR]    = { STR("elr"),                                  },
    [DK_BC_OPCODE_NOT]   = { STR("ikke"),                                 },

    [DK_BC_OPCODE_CALL]  = { STR("kald"),     DK_BC_OPERAND_KIND_SYM     },
    [DK_BC_OPCODE_RET]   = { STR("tilbage"),                                 },
    [DK_BC_OPCODE_BR]    = { STR("gren"),     DK_BC_OPERAND_KIND_POS     },
#endif
};

typedef struct dk_bc_symbol dk_bc_symbol;
struct dk_bc_symbol {
    u64 id;
    u64 size;
    u64 pos;
};

////////////////////////////////////////////////////////////////
//
//
// Dynamic buffer
//
//
////////////////////////////////////////////////////////////////

typedef struct dk_buffer dk_buffer;
struct dk_buffer {
    u8 *data;
    u64 capacity;
    u64 size;
};

static void *   dk_buffer_push(dk_buffer *buffer, u64 size);
static u8 *     dk_buffer_push_u8(dk_buffer *buffer, u8 a);
static u16 *    dk_buffer_push_u16(dk_buffer *buffer, u16 a);
static u32 *    dk_buffer_push_u32(dk_buffer *buffer, u32 a);
static u64 *    dk_buffer_push_u64(dk_buffer *buffer, u64 a);
#define         dk_buffer_push_struct(buffer, T) ((T *)dk_buffer_push((buffer), sizeof(T)))

static void *   dk_buffer_pop(dk_buffer *buffer, u64 size);
static u8       dk_buffer_pop_u8(dk_buffer *buffer);
static u16      dk_buffer_pop_u16(dk_buffer *buffer);
static u32      dk_buffer_pop_u32(dk_buffer *buffer);
static u64      dk_buffer_pop_u64(dk_buffer *buffer);
#define         dk_buffer_pop_struct(buffer, T) ((T *)dk_buffer_pop((buffer), sizeof(T)))

static void *   dk_buffer_get(dk_buffer *buffer, u64 pos, u64 size);
static u8 *     dk_buffer_get_u8(dk_buffer *buffer, u64 pos);
static u16 *    dk_buffer_get_u16(dk_buffer *buffer, u64 pos);
static u32 *    dk_buffer_get_u32(dk_buffer *buffer, u64 pos);
static u64 *    dk_buffer_get_u64(dk_buffer *buffer, u64 pos);
#define         dk_buffer_get_struct(buffer, pos, T) ((T *)dk_buffer_get((buffer), pos))

static void *   dk_buffer_read(dk_buffer *buffer, u64 *pos, u64 size);
static u8       dk_buffer_read_u8(dk_buffer *buffer, u64 *pos);
static u16      dk_buffer_read_u16(dk_buffer *buffer, u64 *pos);
static u32      dk_buffer_read_u32(dk_buffer *buffer, u64 *pos);
static u64      dk_buffer_read_u64(dk_buffer *buffer, u64 *pos);
#define         dk_buffer_read_struct(buffer, pos, T) ((T *)dk_buffer_read(buffer, pos, sizeof(T)))

////////////////////////////////////////////////////////////////
//
//
// Bytecode emit
//
//
////////////////////////////////////////////////////////////////

typedef struct dk_program dk_program;
struct dk_program {
    dk_buffer head;
    dk_buffer body;
};

typedef struct dk_emitter dk_emitter;
struct dk_emitter {
    dk_buffer head;
    dk_buffer body;

    dk_symbol_table symbol_table;
};

typedef struct dk_compiler_local dk_compiler_local;
struct dk_compiler_local {
    str name;
    str type;
    u16 off;
    dk_compiler_local *next;
};

typedef struct dk_compiler_local_list dk_compiler_local_list;
struct dk_compiler_local_list {
    dk_compiler_local *first;
    dk_compiler_local *last;
    u8 count;
};

static arena dk_bc_arena = { 0 };

//- High level api.
static dk_program   dk_program_from_ast(dk_ast ast);
static dk_program   dk_program_from_str(str s, dk_err_sink *err);

//- Helpers.
// TODO(rune): Symbol resolution should be handled in type-check pass.
// TODO(rune): In general the emit pass just needs a cleanup.
static void         dk_compiler_add_local(dk_compiler_local_list *locals, str name, str type);
static u16          dk_compiler_get_local(dk_compiler_local_list locals, str name);
static dk_symbol *  dk_resolve_symbol(dk_emitter *e, str name);
static u32          dk_resolve_symbol_id(dk_emitter *e, str name);

//- Low-level emit helpers.
static u8 *         dk_emit_u8(dk_emitter *e, u8 a);
static u16 *        dk_emit_u16(dk_emitter *e, u16 a);
static u32 *        dk_emit_u32(dk_emitter *e, u32 a);
static u64 *        dk_emit_u64(dk_emitter *e, u64 a);
static void         dk_emit_inst1(dk_emitter *e, dk_bc_opcode opcode);
static void         dk_emit_inst2(dk_emitter *e, dk_bc_opcode opcode, u64 operand);
static void         dk_emit_symbol(dk_emitter *e, u32 id, u32 size);

//- AST emit functions.
static void         dk_emit_literal(dk_emitter *e, dk_ast_literal *literal);
static void         dk_emit_expr(dk_emitter *e, dk_ast_expr *expr, dk_compiler_local_list *locals);
static void         dk_emit_stmts(dk_emitter *e, dk_ast_stmts stmts, dk_compiler_local_list *locals);
static void         dk_emit_proc(dk_emitter *e, dk_ast_proc *proc);

////////////////////////////////////////////////////////////////
//
//
// Runtime
//
//
////////////////////////////////////////////////////////////////

static str dk_run_program(dk_program program, arena *output_arena);

////////////////////////////////////////////////////////////////
//
//
// Debug printing
//
//
////////////////////////////////////////////////////////////////

//- Tokenization
static void dk_print_token(dk_token *token);

//- Abstract syntax tree
static void dk_print_ast(dk_ast ast);
static void dk_print_ast_literal(dk_ast_literal *literal, u64 level);
static void dk_print_ast_expr(dk_ast_expr *expr, u64 level);
static void dk_print_ast_stmt(dk_ast_stmt *stmt, u64 level);
static void dk_print_ast_stmts(dk_ast_stmts stmts, u64 level);
static void dk_print_ast_symbol(dk_ast_symbol *symbol, u64 level);
static void dk_print_ast_level(u64 level);

//- Bytecode
static void dk_print_program(dk_program program);