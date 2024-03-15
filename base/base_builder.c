////////////////////////////////////////////////////////////////
//
//
// String builder / buffer chain
//
//
////////////////////////////////////////////////////////////////

static void builder_free(builder *builder) {
    arena_free(&builder->arena);
}

static builder_block *builder_add_block(builder *builder, u64 size) {
    arena *arena = &builder->arena;
    builder_block *block = arena_push_struct(arena, builder_block);
    if (block) {
        block->buf = arena_push_buf(arena, size);
        slist_add(&builder->blocks, block);
    }

    if (!block || !block->buf.v) {
        builder->oom = true;
    }

    return block;
}

static void builder_append_str(builder *builder, str s) {
    if (!builder->oom) {
        u64 next_block_size = 0;
        if (!builder->blocks.first) {
            next_block_size = kilobytes(8);
        } else if (buf_remaining_size(builder->blocks.last->buf) < s.len) {
            next_block_size = builder->blocks.last->buf.cap * 2;
        }

        if (next_block_size) {
            max_assign(&next_block_size, u64_round_up_to_pow2(s.len));
            builder_add_block(builder, next_block_size);
        }
    }

    if (!builder->oom) {
        assert(buf_remaining_size(builder->blocks.last->buf) >= s.len);
        buf_append_str(&builder->blocks.last->buf, s);
        builder->len += s.len;
    }
}

static void builder_append_u8(builder *builder, u8 c) {
    str s = make_str(&c, 1);
    builder_append_str(builder, s);
}

static void builder_append_utf8_codepoint(builder *builder, u32 c) {
    u8 temp[4];
    i32 units = encode_single_utf8_codepoint(c, temp);
    span s = make_span(u8, temp, units);
    builder_append_str(builder, s);
}

static void builder_append_utf16_codepoint(builder *builder, u32 c) {
    u16 temp[2];
    i32 units = encode_single_utf16_codepoint(c, temp);
    wstr w = make_wstr(temp, units);
    span s = wstr_as_span(w);
    builder_append_str(builder, s);
}

static str builder_to_str(builder *builder, arena *out) {
    buf out_buf = arena_push_buf(out, builder->len);
    for_list (builder_block, it, builder->blocks) {
        str it_str = buf_as_str(it->buf);
        buf_append_str(&out_buf, it_str);
    }
    str ret = buf_as_str(out_buf);
    return ret;
}

static str builder_to_str_and_free(builder *builder, arena *out) {
    str ret = builder_to_str(builder, out);
    builder_free(builder);
    zero(builder);
    return ret;
}
