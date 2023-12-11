////////////////////////////////////////////////////////////////
//
//
// Decode/encode
//
//
////////////////////////////////////////////////////////////////

typedef struct unicode_codepoint unicode_codepoint;
struct unicode_codepoint {
    u32 codepoint;
    u32 len;
};

static i32 get_utf8_class_u8(u8 codeunit);
static i32 get_utf8_class_u32(u32 codepoint);
static i32 get_utf16_class_u16(u16 codeunit);
static i32 get_utf16_class_u32(u32 codepoint);
static bool is_utf8_continuation_byte(u8 codeunit);
static bool is_utf16_continuation(u8 codeunit);
static unicode_codepoint decode_single_utf8_codepoint(str s);
static unicode_codepoint decode_single_utf16_codepoint(wstr w);
static i32 encode_single_utf8_codepoint(u32 codepoint, u8 *out);
static i32 encode_single_utf16_codepoint(u32 codepoint, u16 *out);
static bool advance_single_utf8_codepoint(str *s, unicode_codepoint *decoded);
static bool advance_single_utf16_codepoint(wstr *w, unicode_codepoint *decoded);
static bool is_well_formed_utf8(str s);
static bool is_well_formed_utf16(wstr s);

////////////////////////////////////////////////////////////////
//
//
// Buffer
//
//
////////////////////////////////////////////////////////////////

static void buffer_append_utf8_codepoint(buffer *buffer, u32 append);
static void buffer_append_utf16_codepoint(buffer *buffer, u32 append);

////////////////////////////////////////////////////////////////
//
//
// Conversions
//
//
////////////////////////////////////////////////////////////////

// Conversion results are zero-terminated.
static str convert_utf16_to_utf8(wstr w, arena *out);
static wstr convert_utf8_to_utf16(str s, arena *out);

////////////////////////////////////////////////////////////////
//
//
// Iterator
//
//
////////////////////////////////////////////////////////////////

typedef struct utf8_iter utf8_iter;
struct utf8_iter {
    str s;
    ix idx;
    u32 codepoint;
    bool valid;
};

static utf8_iter utf8_iter_begin(str s, ix idx);
static utf8_iter utf8_iter_next(utf8_iter iter, i32 dir);
