////////////////////////////////////////////////////////////////
//
//
// String builder / buffer chain
//
//
////////////////////////////////////////////////////////////////

typedef struct builder_block builder_block;
struct builder_block {
    buf buf;
    builder_block *next;
};

typedef struct builder builder;
struct builder {
    arena arena;
    u64 len;
    bool oom; // Out of memory.
    list(builder_block) blocks;
};

static void builder_free(builder *builder);
static void builder_append_str(builder *builder, str s);
static void builder_append_u8(builder *builder, u8 c);
static void builder_append_utf8_codepoint(builder *builder, u32 c);
static void builder_append_utf16_codepoint(builder *builder, u32 c);
static void builder_print_args(builder *builder, args args);
#define builder_print(builder, ...) (builder_print_args(builder, argsof(__VA_ARGS__)))
static str builder_to_str(builder *builder, arena *out);
static str builder_to_str_and_free(builder *builder, arena *out);
