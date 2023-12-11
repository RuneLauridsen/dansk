////////////////////////////////////////////////////////////////
//
//
// Ascii
//
//
////////////////////////////////////////////////////////////////

static bool is_ascii_letter(u32 c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
static bool is_ascii_digit(u32 c) { return (c >= '0' && c <= '9'); }
static bool is_ascii_upper(u32 c) { return (c >= 'A' && c <= 'Z'); }
static bool is_ascii_lower(u32 c) { return (c >= 'a' && c <= 'z'); }
static bool is_ascii_non_printable(u32 c) { return (c >= 0   && c <= 31); }
static bool is_ascii_printable(u32 c) { return (c >= 32  && c <= 127); }
static bool is_ascii_one_of(u32 c, str of) {
    bool ret = false;
    for_range(ix, i, 0, of.count) {
        ret |= (c == of.v[i]);
    }
    return ret;
}

static u32 to_ascii_lower(u32 c) { return is_ascii_upper(c) ? c + 32 : c; }
static u32 to_ascii_upper(u32 c) { return is_ascii_lower(c) ? c - 32 : c; }

static bool u32_is_whitespace(u32 c) {
    bool ret = false;

    ret |= (c == ' ');
    ret |= (c == '\n');
    ret |= (c == '\r');
    ret |= (c == '\t');

    return ret;
}

static bool char_is_alpha(char c) {
    bool ret = false;

    ret |= (c >= 'A' && c <= 'Z');
    ret |= (c >= 'a' && c <= 'z');

    return ret;
}

static bool u32_is_digit(char c) {
    bool ret = false;

    ret |= (c >= '0' && c <= '9');

    return ret;
}

static bool u32_is_word(char c) {
    bool ret = false;

    ret |= char_is_alpha(c);
    ret |= (c == '_');

    return ret;
}

static bool u32_is_wordsep(u32 c) {
    bool ret = ((c == ':') || (c == ';') ||
                (c == '(') || (c == ')') ||
                (c == '[') || (c == ']') ||
                (c == '}') || (c == '{') ||
                (c == '.') || (c == ',') ||
                (c == '+') || (c == '-') ||
                (c == '*') || (c == '^') ||
                (c == '<') || (c == '>') ||
                (c == '=') || (c == '!') ||
                (c == '/') || (c == '\\'));
    return ret;
}

static char char_to_upper(char c) {
    if (c >= 'a' && c <= 'z') c -= ('a' - 'A');
    return c;
}

static char char_to_lower(char c) {
    if (c >= 'A' && c <= 'Z') c += ('a' - 'A');
    return c;
}

////////////////////////////////////////////////////////////////
//
//
// Zero terminated string
//
//
////////////////////////////////////////////////////////////////

static ux cstr_len(char *s) {
    char *c = s;

    while (*c) {
        c++;
    }

    ux ret = c - s;
    return ret;
}

static bool cstr_eq(char *a, char *b) {
    bool ret = false;

    if (a && b) {
        while ((*a) && (*a == *b)) {
            a++;
            b++;
        }

        ret = (*a == *b);
    }

    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Counted string
//
//
////////////////////////////////////////////////////////////////

static str str_from_cstr(char *cstr) {
    str ret = make_str((u8 *)cstr, strlen(cstr));
    return ret;
}

static bool str_eq(str a, str b) {
    bool ret = false;

    if (a.count == b.count) {
        if (a.count) {
            ret = memcmp(a.v, b.v, a.count) == 0;
        } else {
            ret = true;
        }
    }

    return ret;
}

static ix str_idx_of_u8(str s, u8 c) {
    ix ret = -1;

    u8 *found = memchr(s.v, c, s.count);
    if (found) {
        ret = found - s.v;
    }

    return ret;
}

static ix str_idx_of_str(str a, str b) {
    ix ret = -1;

    if (b.count == 0) {
        ret = 0;
    } else if (a.count >= b.count) {
        char first = b.v[0];
        char last  = b.v[b.count - 1];

        for (ix i = 0; i <= a.count - b.count; i++) {
            if ((a.v[i] == first) && (a.v[i + b.count - 1] == last)) {
                if (memcmp(a.v + i, b.v, b.count) == 0) {
                    ret = i;
                    break;
                }
            }
        }
    }

    return ret;
}

static bool u8_eq_nocase(u8 a, u8 b) {
    return char_to_upper(a) == char_to_upper(b);
}

static ix str_idx_of_str_nocase(str a, str b) {
    ix ret = -1;

    if (b.count == 0) {
        ret = 0;
    } else if (a.count >= b.count) {
        char first = b.v[0];
        char last  = b.v[b.count - 1];

        for (ix i = 0; i <= a.count - b.count; i++) {
            if (u8_eq_nocase(a.v[i], first) && u8_eq_nocase(a.v[i + b.count - 1], last)) {
                if (_memicmp(a.v + i, b.v, b.count) == 0) {
                    ret = i;
                    break;
                }
            }
        }
    }

    return ret;
}

static ix str_idx_of_last_u8(str s, u8 c) {
    ix ret = -1;

    for (ix i = s.count - 1; i >= 0; i--) {
        if (s.v[i] == c) {
            ret = i;
            break;
        }
    }

    return ret;
}

static ix str_idx_of_any(str s, str any) {
    ix ret = -1;

    for (ix i = s.count - 1; i >= 0; i--) {
        if (is_ascii_one_of(s.v[i], any)) {
            ret = i;
            break;
        }
    }

    return ret;
}

static str substr_idx(str s, ix idx) {
    str ret = str("");

    if (s.count > idx) {
        ret.v     = s.v + idx;
        ret.count = s.count - idx;
    }

    return ret;
}

static str substr_len(str s, ix idx, ix length) {
    str ret = str("");

    if (s.count >= idx + length) {
        ret.v     = s.v + idx;
        ret.count = length;
    }

    return ret;
}

static str substr_range(str s, range_ix r) {
    str ret = str("");

    if (r.min >= 0 && r.max <= s.count) {
        ret.v     = s.v + r.min;
        ret.count = r.max - r.min;
    }

    return ret;
}

static str str_left(str s, ix len) {
    str ret = substr_len(s, 0, len);
    return ret;
}

static str str_right(str s, ix len) {
    str ret = substr_idx(s, s.count - len);
    return ret;
}

static str str_chop_by_delim(str *s, str delim) {
    str ret = str("");

    ix idx = str_idx_of_str(*s, delim);
    if (idx != -1) {
        ret    = substr_len(*s, 0, idx);
        *s     = substr_idx(*s, idx + delim.count);
    } else {
        ret = *s;
        *s = str("");
    }

    return ret;
}

static str str_trim(str s) {
    s = str_trim_left(s);
    s = str_trim_right(s);

    return s;
}

static str str_trim_left(str s) {
    while (s.count && u32_is_whitespace(s.v[0])) {
        s.v     += 1;
        s.count -= 1;
    }

    return s;
}

static str str_trim_right(str s) {
    while (s.count && u32_is_whitespace(s.v[s.count - 1])) {
        s.count--;
    }

    return s;
}

static bool str_starts_with(str s, str prefix) {
    ux idx = str_idx_of_str(s, prefix);
    bool ret = idx == 0;
    return ret;
}

static bool str_ends_with(str s, str suffix) {
    // TODO(rune): Optimize -> no need to search through the whole string.

    ix idx = str_idx_of_str(s, suffix);
    bool ret = idx == s.count - suffix.count;
    return ret;
}

static bool str_starts_with_u8(str s, char c) {
    bool ret = false;
    if (s.count > 0) ret = (s.v[0] == c);
    return ret;
}

static bool str_ends_with_u8(str s, char c) {
    bool ret = false;
    if (s.count > 0) ret = (s.v[s.count - 1] == c);
    return ret;
}

static str3 str3_split(str s, ix a, ix b) {
    assert(a <= b);
    str3 ret = { 0 };
    ret.v[0] = substr_len(s, 0, a);
    ret.v[1] = substr_len(s, a, b - a);
    ret.v[2] = substr_idx(s, b);
    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// String builder
//
//
////////////////////////////////////////////////////////////////

static bool builder_create(builder *builder, ux initial_size) {
    builder->length = 0;
    builder->ok = true;
    bool ret = arena_create(&builder->arena, initial_size, false, ARENA_KIND_EXPONENTIAL_CHAIN);
    return ret;
}

static void builder_destroy(builder *builder) {
    free_arena(&builder->arena);
    builder->length = 0;
    builder->ok = false;
}

static void builder_reset(builder *builder) {
    arena_reset(&builder->arena);
    builder->length = 0;
    builder->ok = true;
}

static void builder_append(builder *builder, str s) {
    if (builder->ok && s.count) {
        void *ptr = push_size_nz(&builder->arena, s.count);
        if (ptr) {
            memcpy(ptr, s.v, s.count);
            builder->length += s.count;
        } else {
            builder->ok = false;
        }
    }
}

static void builder_append_c(builder *builder, u8 c) {
    str s = make_str(&c, 1);
    builder_append(builder, s);
}

static void builder_append_f(builder *builder, char *format, ...) {
    if (builder->ok) {
        va_list args;
        va_start(args, format);
        builder_append_v(builder, format, args);
        va_end(args);
    }
}

static void builder_append_v(builder *builder, char *format, va_list args) {
    if (builder->ok) {
        ux length = vsnprintf(null, 0, format, args);

        // NOTE(rune): vsnprintf must have space enough to add a \0, even though we don't need it.
        char *ptr = push_size_pad_nz(&builder->arena, length, 1);

        if (ptr) {
            vsnprintf(ptr, length + 1, format, args);
            builder->length += length;
        } else {
            builder->ok = false;
        }
    }
}

static str builder_to_string(builder *builder, arena *dest_arena) {
    str ret = str("");

    if (builder->ok) {
        u8 *ptr = push_size_nz(dest_arena, builder->length + 1);
        if (ptr) {
            ux current_length = 0;

            for (arena_block *block = builder->arena.first_block; block; block = block->next) {
                u8 *src      = (u8 *)block;
                ix  src_size = block->occupied;

                src      += sizeof(arena_block);
                src_size -= sizeof(arena_block);

                assert(current_length + src_size <= builder->length);
                memcpy(ptr + current_length, src, src_size);
                current_length += src_size;
            }

            assert(current_length == builder->length);

            ret.v     = ptr;
            ret.count = current_length;
            ptr[current_length] = 0;
        }
    }

    return ret;
}

static str builder_to_string_and_destroy(builder *builder, arena *arena) {
    str ret = builder_to_string(builder, arena);
    builder_destroy(builder);
    return ret;
}

static str builder_to_string_and_reset(builder *builder, arena *arena) {
    str ret = builder_to_string(builder, arena);
    builder_reset(builder);
    return ret;
}
