////////////////////////////////////////////////////////////////
//
//
// Buffer
//
//
////////////////////////////////////////////////////////////////

static buffer make_buffer(span span) {
    buffer ret = { 0 };
    ret.v = span.v;
    ret.cap = span.len;
    ret.count = 0;
    return ret;
}

static buffer push_buffer(arena *arena, ix cap) {
    span span = push_span(arena, u8, cap);
    buffer ret = make_buffer(span);
    return ret;
}

static span buffer_as_span(buffer buffer) {
    span ret = make_span(u8, buffer.v, buffer.count);
    return ret;
}

static void buffer_reset(buffer *buffer) {
    buffer->count = 0;
}

static str buffer_as_str(buffer buffer) {
    str ret = make_str(buffer.v, buffer.count);
    return ret;
}

static wstr buffer_as_wstr(buffer buffer) {
    assert(buffer.count % 2 == 0);
    wstr ret = make_wstr((u16 *)buffer.v, buffer.count / 2);
    return ret;
}

static void buffer_append_u8(buffer *buffer, u8 append) {
    if (buffer->count < buffer->cap) {
        buffer->v[buffer->count++] = append;
    }
}

static void buffer_append_str(buffer *buffer, str append) {
    clamp_assign(&append.len, 0, buffer->cap - buffer->count);
    memcpy(buffer->v + buffer->count, append.v, append.len);
    buffer->count += append.len;
}

static void buffer_replace(buffer *buffer, range_ix replace, str replace_with) {
    ix replace_len = range_len(replace);
    clamp_assign(&replace_with.len, 0, buffer->cap - buffer->count + replace_len);

    u8 *move_dst    = buffer->v + replace.min + replace_with.len;
    u8 *move_src    = buffer->v + replace.max;
    ix  move_size   = buffer->count - replace.max;

    memmove(move_dst, move_src, move_size);
    memcpy(buffer->v + replace.min, replace_with.v, replace_with.len);

    buffer->count -= replace_len;
    buffer->count += replace_with.len;
}

static void buffer_delete(buffer *buffer, range_ix range) {
    buffer_replace(buffer, range, str(""));
}

static void buffer_insert(buffer *buffer, ix insert_at, str insert) {
    buffer_replace(buffer, range_ix(insert_at, insert_at), insert);
}

static void buffer_null_terminate_u8(buffer *buffer) {
    if (buffer->count < buffer->cap) {
        buffer->v[buffer->count] = '\0';
    } else if (buffer->cap > 0) {
        buffer->v[buffer->cap - 1] = '\0';
    }
}

static void buffer_null_terminate_u16(buffer *buffer) {
    if (buffer->count + 1 < buffer->cap) {
        buffer->v[buffer->count] = '\0';
        buffer->v[buffer->count + 1] = '\0';
    } else if (buffer->cap > 0) {
        buffer->v[buffer->cap - 2] = '\0';
        buffer->v[buffer->cap - 1] = '\0';
    }
}
