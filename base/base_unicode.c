////////////////////////////////////////////////////////////////
//
//
// Decode/encode
//
//
////////////////////////////////////////////////////////////////

static readonly u8 utf8_table[256] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0-1f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 20-3f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 40-5f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 60-7f
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80-9f
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // a0-bf
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // c0-df
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, // e0-ff
};

static i32 get_utf8_class_u8(u8 cu) {
    assert(cu < countof(utf8_table));
    i32 ret = (i32)utf8_table[cu];
    return ret;
}

static i32 get_utf8_class_u32(u32 cp) {
    if (0)                   return 0;
    else if (cp <= 0x7F)     return 1;
    else if (cp <= 0x7FF)    return 2;
    else if (cp <= 0xFFFF)   return 3;
    else if (cp <= 0x10FFFF) return 4;
    else                     return 0;
}

static i32 get_utf16_class_u32(u32 codepoint) {
    return 1; // @Implement
}

static bool is_utf8_continuation_byte(u8 codeunit) {
    bool ret = get_utf8_class_u8(codeunit) == 0;
    return ret;
}

static unicode_codepoint decode_single_utf8_codepoint(str s) {
    // https://en.wikipedia.org/wiki/UTF-8#Encoding

    //
    // NOTE(rune): Table generated with:
    //
#if 0
#include <stdio.h>
    int main() {
        printf("static const u8 decode_table[256] =\n{");

        for (int i = 0; i < 256; i++) {
            if (i % 32 == 0) printf("\n    ");

            if ((i >> 3) == 0b11110)      printf("4, "); // 11110xxx
            else if ((i >> 4) == 0b1110)  printf("3, "); // 1110xxxx
            else if ((i >> 5) == 0b110)   printf("2, "); // 111xxxxx
            else if ((i >> 7) == 0)       printf("1, "); // 0xxxxxxx
            else                         printf("0, "); // continuation
        }

        printf("\n};\n");
    }
#endif

    unicode_codepoint ret = { 0 };

    //
    // (rune): 1-byte encoding
    //

    if ((s.count >= 1) &&
        (utf8_table[s.v[0]] == 1)) {
        ret.len = 1;
        ret.codepoint = s.v[0];
    }

    //
    // (rune): 2-byte encoding
    //

    if ((s.count >= 2) &&
        (utf8_table[s.v[0]] == 2) &&
        (utf8_table[s.v[1]] == 0)) {
        ret.len = 2;
        ret.codepoint = (((s.v[0] & 0b0001'1111) << 6) |
                         ((s.v[1] & 0b0011'1111) << 0));
    }

    //
    // (rune): 3-byte encoding
    //

    if ((s.count >= 3) &&
        (utf8_table[s.v[0]] == 3) &&
        (utf8_table[s.v[1]] == 0) &&
        (utf8_table[s.v[2]] == 0)) {
        ret.len = 3;
        ret.codepoint = (((s.v[0] & 0b0000'1111) << 12) |
                         ((s.v[1] & 0b0011'1111) <<  6) |
                         ((s.v[2] & 0b0011'1111) <<  0));
    }

    //
    // (rune): 4-byte encoding
    //

    if ((s.count >= 4) &&
        (utf8_table[s.v[0]] == 4) &&
        (utf8_table[s.v[1]] == 0) &&
        (utf8_table[s.v[2]] == 0) &&
        (utf8_table[s.v[3]] == 0)) {
        ret.len = 4;
        ret.codepoint = (((s.v[0] & 0b0000'0111) << 18) |
                         ((s.v[1] & 0b0011'1111) << 12) |
                         ((s.v[2] & 0b0011'1111) <<  6) |
                         ((s.v[3] & 0b0011'1111) <<  0));
    }

    return ret;
}

static i32 encode_single_utf8_codepoint(u32 c, u8 *out) {
    i32 ret = 0;
    switch (get_utf8_class_u32(c)) {
        case 1: {
            out[0] = (u8)c;
            ret = 1;
        } break;

        case 2: {
            out[0] = 0b1100'0000 | ((c >> 6) & 0b0001'1111);
            out[1] = 0b1000'0000 | ((c >> 0) & 0b0011'1111);
            ret = 2;
        } break;

        case 3: {
            out[0] = 0b1110'0000 | ((c >> 12) & 0b0000'1111);
            out[1] = 0b1000'0000 | ((c >>  6) & 0b0011'1111);
            out[2] = 0b1000'0000 | ((c >>  0) & 0b0011'1111);
            ret = 3;
        } break;

        case 4: {
            out[0] = 0b1111'0000 | ((c >> 18) & 0b0000'0111);
            out[1] = 0b1000'0000 | ((c >> 12) & 0b0011'1111);
            out[2] = 0b1000'0000 | ((c >>  6) & 0b0011'1111);
            out[3] = 0b1000'0000 | ((c >>  0) & 0b0011'1111);
            ret = 4;
        } break;

        default: {
            out[0] = ' ';
            ret = 1;
        } break;
    }
    return ret;
}

static i32 encode_single_utf16_codepoint(u32 codepoint, u16 *out) {
    // @Implement
    out[0] = u16(codepoint);
    return 1;
}

static bool advance_single_utf8_codepoint(str *s, unicode_codepoint *decoded) {
    bool ret = false;
    if (s->len) {
        *decoded = decode_single_utf8_codepoint(*s);

        s->v     += decoded->len;
        s->count -= decoded->len;

        ret = decoded->len > 0;
    }
    return ret;
}

static unicode_codepoint decode_single_utf16_codepoint(wstr w) {
    // TODO(rune): Better utf16 decoder.

    unicode_codepoint ret = { 0 };

    if (w.count) {
        assert(w.v[0] < 0xd7ff);
        ret.codepoint = w.v[0];
        ret.len = 1;
    }

    return ret;
}

static bool advance_single_utf16_codepoint(wstr *w, unicode_codepoint *decoded) {
    *decoded = decode_single_utf16_codepoint(*w);

    w->v     += decoded->len;
    w->count -= decoded->len;

    bool ret = decoded->len > 0;
    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Conversions
//
//
////////////////////////////////////////////////////////////////

static str convert_utf16_to_utf8(wstr w, arena *out) {
    ix max_len_as_utf8 = (w.count + 1) * 3;
    buffer buffer = push_buffer(out, max_len_as_utf8);
    unicode_codepoint decoded = { 0 };
    while (advance_single_utf16_codepoint(&w, &decoded)) {
        buffer_append_utf8_codepoint(&buffer, decoded.codepoint);
    }

    buffer_null_terminate_u8(&buffer);
    str ret = buffer_as_str(buffer);
    return ret;
}

static wstr convert_utf8_to_utf16(str s, arena *out) {
    ux max_len_as_utf16 = (s.count + 1) * 2;
    buffer buffer = push_buffer(out, max_len_as_utf16);

    unicode_codepoint decoded = { 0 };
    while (advance_single_utf8_codepoint(&s, &decoded)) {
        buffer_append_utf16_codepoint(&buffer, decoded.codepoint);
    }

    buffer_null_terminate_u16(&buffer);
    wstr ret = buffer_as_wstr(buffer);
    return ret;
}

static bool is_well_formed_utf8(str s) {
    ix i = 0;
    str remaining = s;
    unicode_codepoint decoded = { 0 };
    while (advance_single_utf8_codepoint(&remaining, &decoded)) {
        i += decoded.len;
    }

    bool ret = s.count == i;
    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Buffer
//
//
////////////////////////////////////////////////////////////////

static void buffer_append_utf8_codepoint(buffer *buffer, u32 append) {
    i32 num_units = get_utf8_class_u32(append);
    if (buffer->count + num_units <= buffer->cap) {
        encode_single_utf8_codepoint(append, buffer->v + buffer->count);
        buffer->count += num_units;
    }
}

static void buffer_append_utf16_codepoint(buffer *buffer, u32 append) {
    i32 num_units = get_utf16_class_u32(append);
    if (buffer->count + num_units * 2 <= buffer->cap) {
        encode_single_utf16_codepoint(append, (u16 *)(buffer->v + buffer->count));
        buffer->count += num_units * 2;
    }
}

////////////////////////////////////////////////////////////////
//
//
// Unicode iterator
//
//
////////////////////////////////////////////////////////////////

static utf8_iter utf8_iter_begin(str s, ix idx) {
    utf8_iter ret = { 0 };
    ret.s = s;
    ret.idx = idx;
    unicode_codepoint decoded = decode_single_utf8_codepoint(substr_idx(s, idx));
    ret.codepoint = decoded.codepoint;
    ret.valid = idx >= 0 && idx <= s.count - 1;
    return ret;
}

// TODO(rune): Simplify.
static utf8_iter utf8_iter_next(utf8_iter iter, i32 dir) {
    iter.valid = false;
    switch (dir) {
        case 1: {
            if (iter.idx < iter.s.count) {
                i32 utf8_class = get_utf8_class_u8(iter.s.v[iter.idx]);
                iter.idx += utf8_class;
                unicode_codepoint decoded = decode_single_utf8_codepoint(substr_idx(iter.s, iter.idx));
                if (decoded.codepoint) {
                    iter.valid = true;
                    iter.codepoint = decoded.codepoint;
                } else {
                    iter.valid = false;
                    iter.codepoint = 0;
                }
            } else {
                iter.idx++;
            }
        } break;

        case -1: {
            if (iter.idx > 0) {
                while (1) {
                    iter.idx--;
                    if (iter.idx < 0) {
                        break;
                    }

                    i32 utf8_class = get_utf8_class_u8(iter.s.v[iter.idx]);
                    if (utf8_class != 0) {
                        unicode_codepoint decoded = decode_single_utf8_codepoint(substr_idx(iter.s, iter.idx));
                        if (decoded.codepoint) {
                            iter.codepoint = decoded.codepoint;
                            break;
                        }
                    }
                }

                iter.valid = iter.idx >= 0;
            } else {
                iter.idx--;
            }
        } break;
    }

    clamp_assign(&iter.idx, 0, iter.s.len);
    return iter;
}
