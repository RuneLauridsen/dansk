////////////////////////////////////////////////////////////////
//
//
// Ascii
//
//
////////////////////////////////////////////////////////////////

static bool is_ascii_letter(u32 c);
static bool is_ascii_digit(u32 c);
static bool is_ascii_upper(u32 c);
static bool is_ascii_lower(u32 c);
static bool is_ascii_non_printable(u32 c);
static bool is_ascii_printable(u32 c);
static bool is_ascii_one_of(u32 c, str of);

////////////////////////////////////////////////////////////////
//
//
// String operations
//
//
////////////////////////////////////////////////////////////////

static str str_from_cstr(char *cstring);
static bool str_eq(str a, str b);

static ix str_idx_of_u8(str s, u8 c);
static ix str_idx_of_str(str a, str b);
static ix str_idx_of_last_u8(str s, u8 c);
static ix str_idx_of_last_str(str a, str b);

static str substr_idx(str s, ix idx);
static str substr_len(str s, ix idx, ix len);
static str substr_range(str, range_ix r);

static str str_chop_left(str *s, ix idx); // @Implement
static str str_chop_right(str *s, ix idx); // @Implement
static str str_chop_by_delim(str *s, str delim);

static str str_left(str s, ix len);
static str str_right(str s, ix len);
static str str_trim(str s);
static str str_trim_left(str s);
static str str_trim_right(str s);
static bool str_starts_with(str s, str prefix);
static bool str_starts_with_u8(str s, char c);
static bool str_ends_with(str s, str suffix);
static bool str_ends_with_u8(str s, char c);

typedef struct { str v[3]; } str3;
static str3 str3_split(str s, ix a, ix b);

////////////////////////////////////////////////////////////////
//
//
// String builder
//
//
////////////////////////////////////////////////////////////////

typedef struct builder builder;
struct builder {
    arena arena;
    ux length;

    // NOTE(rune): Set to false, whenever an append operation fails. Subsequent append operations will be no-ops.
    bool ok;
};

// TODO(rune): It would be nice to have a builder_init(arena *), in case you want use a temp storage arena.

static bool builder_create(builder *builder, ux initial_size);
static void builder_destroy(builder *builder);
static void builder_reset(builder *builder);
static void builder_append(builder *builder, str s);
static void builder_append_c(builder *builder, u8 c);
static void builder_append_f(builder *builder, char *format, ...);
static void builder_append_v(builder *builder, char *format, va_list args);
static str builder_to_string(builder *builder, arena *arena);
static str builder_to_string_and_destroy(builder *builder, arena *arena);
static str builder_to_string_and_reset(builder *builder, arena *arena);
