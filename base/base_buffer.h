////////////////////////////////////////////////////////////////
//
//
// Buffer
//
//
////////////////////////////////////////////////////////////////

typedef struct buffer buffer;
struct buffer {
    u8 *v;
    union { ix count, len; };
    ix cap;
};

static buffer make_buffer(span span);
static buffer push_buffer(arena *arena, ix cap);
static span buffer_as_span(buffer buffer);
static void buffer_reset(buffer *buffer);
static str buffer_as_str(buffer buffer);
static wstr buffer_as_wstr(buffer buffer);
static void buffer_append_u8(buffer *buffer, u8 append);
static void buffer_append_str(buffer *buffer, str append);
static void buffer_replace(buffer *buffer, range_ix replace, str replace_with);
static void buffer_delete(buffer *buffer, range_ix range);
static void buffer_insert(buffer *buffer, ix insert_at, str insert);
static void buffer_null_terminate_u8(buffer *buffer);
static void buffer_null_terminate_u16(buffer *buffer);
