// @Feature Format specifier for u32 -> hex rgb/rgba.
// @Feature Format specifier for string -> Properly escaped C string literal.
// @Feature Format specifier for char   -> Properly escaped C char literal.
// @Feature Color speicifers that are translated to ANSI escape codes.

// @Todo sprintf always null termiates, even though we don't need it.
static ix sprint_any(u8 *out, ix cap, str format_specifier, any a) {
    ix ret = 0;
    bool fmt_var = str_eq(format_specifier, str("var"));
    bool fmt_c = str_eq(format_specifier, str("c"));

    switch (a.tag) {
        case ANY_TAG_I8:        ret = snprintf((char *)out, cap, fmt_c ? (fmt_var ? "'%c'" : "%c") : "%i", a._i8); break;
        case ANY_TAG_I16:       ret = snprintf((char *)out, cap, fmt_c ? (fmt_var ? "'%c'" : "%c") : "%i", a._i16); break;
        case ANY_TAG_I32:       ret = snprintf((char *)out, cap, fmt_c ? (fmt_var ? "'%c'" : "%c") : "%i", a._i32); break;
        case ANY_TAG_I64:       ret = snprintf((char *)out, cap, "%lli", a._i64); break;
        case ANY_TAG_U8:        ret = snprintf((char *)out, cap, fmt_c ? (fmt_var ? "'%c'" : "%c") : "%u", a._u8); break;
        case ANY_TAG_U16:       ret = snprintf((char *)out, cap, fmt_c ? (fmt_var ? "'%c'" : "%c") : "%u", a._u16); break;
        case ANY_TAG_U32:       ret = snprintf((char *)out, cap, fmt_c ? (fmt_var ? "'%c'" : "%c") : "%u", a._u32); break;
        case ANY_TAG_U64:       ret = snprintf((char *)out, cap, "%llu", a._u64); break;
        case ANY_TAG_F32:       ret = snprintf((char *)out, cap, "%f", a._f32); break;
        case ANY_TAG_F64:       ret = snprintf((char *)out, cap, "%f", a._f64); break;
        case ANY_TAG_BOOL:      ret = snprintf((char *)out, cap, a._bool ? "true" : "false"); break;
        case ANY_TAG_STR:       ret = snprintf((char *)out, cap, fmt_var ? "\"%.*s\"" : "%.*s", (int)a._str.count, a._str.v); break;
        case ANY_TAG_VEC2:      ret = snprintf((char *)out, cap, "(%f, %f)", a._vec2.x, a._vec2.y); break;
        case ANY_TAG_VEC3:      ret = snprintf((char *)out, cap, "(%f, %f, %f)", a._vec3.x, a._vec3.y, a._vec3.z); break;
        case ANY_TAG_VEC4:      ret = snprintf((char *)out, cap, "(%f, %f, %f, %f)", a._vec4.x, a._vec4.y, a._vec4.z, a._vec4.w); break;
        case ANY_TAG_RECT:      ret = snprintf((char *)out, cap, "(%f, %f) x (%f, %f)", a._rect.x0, a._rect.y0, a._rect.x1, a._rect.y1); break;
        case ANY_TAG_RANGE_IX:  ret = snprintf((char *)out, cap, "[%lli, %lli]", a._range_ix.min, a._range_ix.max); break;
        case ANY_TAG_RANGE_I8:  ret = snprintf((char *)out, cap, "[%i, %i]", a._range_i8.min, a._range_i8.max); break;
        case ANY_TAG_RANGE_I16: ret = snprintf((char *)out, cap, "[%i, %i]", a._range_i16.min, a._range_i16.max); break;
        case ANY_TAG_RANGE_I32: ret = snprintf((char *)out, cap, "[%i, %i]", a._range_i32.min, a._range_i32.max); break;
        case ANY_TAG_RANGE_I64: ret = snprintf((char *)out, cap, "[%lli, %lli]", a._range_i64.min, a._range_i64.max); break;
        case ANY_TAG_RANGE_UX:  ret = snprintf((char *)out, cap, "[%llu, %llu]", a._range_ux.min, a._range_ux.max); break;
        case ANY_TAG_RANGE_U8:  ret = snprintf((char *)out, cap, "[%u, %u]", a._range_u8.min, a._range_u8.max); break;
        case ANY_TAG_RANGE_U16: ret = snprintf((char *)out, cap, "[%u, %u]", a._range_u16.min, a._range_u16.max); break;
        case ANY_TAG_RANGE_U32: ret = snprintf((char *)out, cap, "[%u, %u]", a._range_u32.min, a._range_u32.max); break;
        case ANY_TAG_RANGE_U64: ret = snprintf((char *)out, cap, "[%llu, %llu]", a._range_u64.min, a._range_u64.max); break;
        case ANY_TAG_RANGE_F32: ret = snprintf((char *)out, cap, "[%f, %f]", a._range_f32.min, a._range_f32.max); break;
        case ANY_TAG_RANGE_F64: ret = snprintf((char *)out, cap, "[%f, %f]", a._range_f64.min, a._range_f64.max); break;
        case ANY_TAG_CHAR_PTR:  ret = snprintf((char *)out, cap, "%s", a._char_ptr); break;
        case ANY_TAG_VOID_PTR:  ret = snprintf((char *)out, cap, "%p", a._void_ptr); break;
        default:                ret = snprintf((char *)out, cap, "[unknown type]"); break;
    }

    return ret;
}

static any pop_first_arg(args *args) {
    any ret = { 0 };
    if (args->count > 0) {
        ret = args->v[0];
        args->count--;
        args->v++;
    }
    return ret;
}

static ix sprint_args(u8 *out, ix cap, args args) {
    // First argument must be format string.
    any fmt_arg = pop_first_arg(&args);
    assert(fmt_arg.tag == ANY_TAG_CHAR_PTR);
    char *fmt = fmt_arg._char_ptr;

    ix len = 0;
    bool is_specifier = false;
    i32 arg_idx = 0;
    while (!out || len < cap) {
        if (is_specifier) {
            any a;
            if (arg_idx < args.count) {
                a = args.v[arg_idx];
            } else {
                a = anyof("[not enough arguments]");
            }

            str format_specifier = str("");
            if (fmt[0] == 'v' &&
                fmt[1] == 'a' &&
                fmt[2] == 'r') {
                fmt += 3;
                format_specifier = str("var");
            }

            if (fmt[0] == 'c') {
                fmt += 1;
                format_specifier = str("c");
            }

            u8 *at    = out ? out + len : null;
            ix at_cap = out ? cap - len : 0;
            ix arg_len = sprint_any(at, at_cap, format_specifier, a);
            len += arg_len;

            is_specifier = false;
            arg_idx++;
        }

        char c = *fmt++;
        if (c == '\0') {
            break;
        }

        if (!is_specifier) {
            if (c == '%') {
                is_specifier = true;
            } else {
                if (out && len < cap) {
                    out[len] = c;
                }

                len++;
            }
        }
    }

    if (out) {
        len = clamp_top(len, cap);
    }

    return len;
}

static void print_args(args args) {
    // @Cleanup
    u8 scratch[4096];

    ix len = sprint_args(scratch, sizeof(scratch) - 1, args);
    scratch[len] = '\0';
    printf("%s", (char *)scratch);
}

static void println_args(args args) {
    print_args(args);
    print_args(argsof("\n"));
}

static str push_print_args(arena *arena, args args) {
    ix len = sprint_args(0, 0, args);
    str ret = { 0 };
    ret.v = push_size_nz(arena, len + 1);
    if (ret.v) {
        sprint_args(ret.v, len + 1, args);
        ret.len = len;
        ret.v[len] = '\0';
    }
    return ret;
}
