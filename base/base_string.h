////////////////////////////////////////////////////////////////
//
//
// Char flags
//
//
////////////////////////////////////////////////////////////////

typedef enum char_flags {
    CHAR_FLAG_CONTROL    = (1 << 0),
    CHAR_FLAG_PRINTABLE  = (1 << 1),
    CHAR_FLAG_WHITESPACE = (1 << 2),
    CHAR_FLAG_UPPER      = (1 << 3),
    CHAR_FLAG_LOWER      = (1 << 4),
    CHAR_FLAG_LETTER     = (1 << 5),
    CHAR_FLAG_DIGIT      = (1 << 6),
    CHAR_FLAG_HEXDIGIT   = (1 << 7),
    CHAR_FLAG_PUNCT      = (1 << 8),
    CHAR_FLAG_UNDERSCORE = (1 << 9),
    CHAR_FLAG_NON_ASCII  = (1 << 10),

    CHAR_FLAG_WORD       = CHAR_FLAG_LETTER|CHAR_FLAG_DIGIT|CHAR_FLAG_UNDERSCORE,
    CHAR_FLAG_WORDSEP    = CHAR_FLAG_PUNCT|CHAR_FLAG_WHITESPACE,
} char_flags;

static char_flags u32_get_char_flags(u32 c);
static char_flags u8_get_char_flags(u8 c);

////////////////////////////////////////////////////////////////
//
//
// Ascii
//
//
////////////////////////////////////////////////////////////////

static bool u32_is_ascii_letter(u32 c);
static bool u32_is_ascii_digit(u32 c);
static bool u32_is_ascii_upper(u32 c);
static bool u32_is_ascii_lower(u32 c);
static bool u32_is_ascii_non_printable(u32 c);
static bool u32_is_ascii_printable(u32 c);
static bool u32_is_ascii_one_of(u32 c, str of);

////////////////////////////////////////////////////////////////
//
//
// String operations
//
//
////////////////////////////////////////////////////////////////

static str str_from_cstr(char *cstr);
static bool str_eq(str a, str b);
static bool str_eq_nocase(str a, str b);

static u64_result str_idx_of_u8(str s, u8 c);
static u64_result str_idx_of_str(str a, str b);
static u64_result str_idx_of_last_u8(str s, u8 c);
static u64_result str_idx_of_last_str(str a, str b);

static str substr_idx(str s, u64 idx);
static str substr_len(str s, u64 idx, u64 len);
static str substr_range(str s, u64_range r);

static str str_chop_left(str *s, u64 idx); // @Implement
static str str_chop_right(str *s, u64 idx); // @Implement
static str str_chop_by_delim(str *s, str delim);

static str str_left(str s, u64 len);
static str str_right(str s, u64 len);
static str str_trim(str s);
static str str_trim_left(str s);
static str str_trim_right(str s);
static bool str_starts_with_str(str s, str prefix);
static bool str_starts_with_u8(str s, char c);
static bool str_ends_with_str(str s, str suffix);
static bool str_ends_with_u8(str s, char c);

typedef struct { str v[3]; } str_x3;
static str_x3 str_split_x3(str s, u64 a, u64 b);

////////////////////////////////////////////////////////////////
//
//
// Fuzzy
//
//
////////////////////////////////////////////////////////////////

typedef struct str_node str_node;
struct str_node {
    str v;
    str_node *next;
};

typedef struct str_list str_list;
struct str_list {
    str_node *first;
    str_node *last;
    u64 count;
};

typedef struct fuzzy_match_node fuzzy_match_node;
struct fuzzy_match_node {
    u64_range range;
    fuzzy_match_node *next;
};

typedef struct fuzzy_match_list fuzzy_match_list;
struct fuzzy_match_list {
    fuzzy_match_node *first;
    u64 count;
};
