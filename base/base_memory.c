////////////////////////////////////////////////////////////////
//
//
// Allocation tracking
//
//
////////////////////////////////////////////////////////////////

#ifdef TRACK_ALLOCATIONS
#include "thirdparty/idk.h"
#include "thirdparty/idk.c"
#endif

////////////////////////////////////////////////////////////////
//
//
// Heap
//
//
////////////////////////////////////////////////////////////////

static void *heap_alloc(ix size, bool init_to_zero) {
    void *ret = HeapAlloc(GetProcessHeap(), init_to_zero ? HEAP_ZERO_MEMORY : 0, size);
#ifdef TRACK_ALLOCATIONS
    idk_track_alloc(IDK_LOCATION, ret, size);
#endif
    return ret;
}

static void *heap_realloc(void *p, ix size, bool init_to_zero) {
    void *ret = HeapReAlloc(GetProcessHeap(), init_to_zero ? HEAP_ZERO_MEMORY : 0, p, size);
#ifdef TRACK_ALLOCATIONS
    idk_track_realloc(IDK_LOCATION, p, ret, size);
#endif
    return ret;
}

static void heap_free(void *p) {
    HeapFree(GetProcessHeap(), 0, p);
#ifdef TRACK_ALLOCATIONS
    idk_track_free(IDK_LOCATION, p);
#endif
}

static span(void) heap_alloc_span_(ix count, ix elemsize, bool init_to_zero) {
    span_void ret = { 0 };
    ret.v = heap_alloc(count * elemsize, init_to_zero);
    if (ret.v) {
        ret.count = count;
    }
    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Arena
//
//
////////////////////////////////////////////////////////////////

static arena_block *next_arena_block(arena *arena, ix want_size) {
    arena_block *ret = arena->first_block;
    ix next_capacity = megabytes(1);
    if (arena->current_block) {
        ret = arena->current_block->next;
        if (arena->kind == ARENA_KIND_EXPONENTIAL_CHAIN) {
            next_capacity = arena->current_block->capacity * 2;
        } else {
            next_capacity = arena->current_block->capacity;
        }

        next_capacity = max(next_capacity, ix(want_size + sizeof(arena_block)));
        next_capacity = u64_to_power_of_two(next_capacity);
    }

    if (!ret) {
        void *new_memory = heap_alloc(next_capacity, false);
        if (new_memory) {
            // NOTE(rune): Store the new chain block inside itself.
            ret = (arena_block *)new_memory;
            zero(ret);
            ret->capacity = next_capacity;
        }
    }

    if (ret) {
        dlist_stack_push(arena->current_block, ret);
        ret->occupied = sizeof(*ret);

        if (!arena->first_block) {
            arena->first_block = ret;
        }
    }

    return ret;
}

static void arena_init(arena *arena, void *memory, ix size) {
    zero(arena);
    unused(size, memory);
}

static bool arena_create(arena *arena, ix size, bool init_to_zero, arena_kind kind) {
    zero(arena);
    arena->kind = kind;

    // @Todo Respect init_to_zero argument.
    // @Todo Respect size argument. Current defaults to whatever is hardcoded in next_arena_block()
    unused(init_to_zero, size);
    next_arena_block(arena, 0);
    return true;
}

static void free_arena(arena *arena) {
    arena_block *block = arena->first_block;
    while (block) {
        // NOTE(rune): We need to store to next pointer locally on the stack, since
        // arena blocks are stored inside themselves, and their next pointers would
        // therefore be invalid, after the chain block's memory is freed.
        arena_block *next = next = block->next;
        heap_free(block);
        block = next;
    }

    zero(arena);
}

static ix arena_size_used(arena *arena) {
    ix ret = 0;
    for_ptr_list(arena_block, it, arena->first_block) {
        ret += it->occupied;
    }
    return ret;
}

static void arena_reset(arena *arena) {
    arena->current_block = null;
}

static bool fits_in_arena_block(arena_block *block, ix size) {
    bool ret = false;
    if (block) {
        ret = block->capacity >= block->occupied + size;
    }
    return ret;
}

static void *push_size_pad_nz(arena *arena, ix size, ix pad) {
    arena_block *block = arena->current_block;
    if (!fits_in_arena_block(block, size + pad)) {
        block = next_arena_block(arena, size + pad);
    }

    void *ret = null;
    if (fits_in_arena_block(block, size + pad)) {
        ret = (u8 *)block + block->occupied;
        block->occupied += size;
    }

    return ret;
}

static void *push_padded_size(arena *arena, ix size, ix pad) {
    void *ret = push_size_pad_nz(arena, size, pad);
    if (ret) {
        memset(ret, 0, size);
    }
    return ret;
}

static void *push_size_nz(arena *arena, ix size) {
    void *ret = push_size_pad_nz(arena, size, 0);
    return ret;
}

static void *push_size(arena *arena, ix size) {
    void *ret = push_padded_size(arena, size, 0);
    return ret;
}

static u8 *arena_top(arena *arena) {
    u8 *ret = (u8 *)push_size(arena, 0);
    return ret;
}

static str copy_str_null_terminate(arena *arena, str s) {
    str ret = { 0 };
    ret.v   = (u8 *)push_size_nz(arena, s.len + 1);

    if (ret.v) {
        memcpy(ret.v, s.v, s.len);
        ret.len = s.len;
        ret.v[ret.len] = '\0';
    }

    return ret;
}

static span_void push_span_(arena *arena, ix count, ix elemsize) {
    span_void ret = { 0 };
    ret.v = push_size(arena, count * elemsize);
    ret.count = ret.v ? count : 0;
    return ret;
}

static span_void push_span_nz_(arena *arena, ix count, ix elemsize) {
    span_void ret = { 0 };
    ret.v = push_size_nz(arena, count * elemsize);
    ret.count = ret.v ? count : 0;
    return ret;
}

static span_void copy_span_(arena *arena, span_void src, ix elemsize) {
    span_void ret = push_span_(arena, src.count, elemsize);
    if (ret.v) {
        memcpy(ret.v, src.v, src.count * elemsize);
        ret.count = src.count;
    }

    return ret;
}

#ifndef __cplusplus
static str copy_str(arena *arena, str s) {
    return copy_span(arena, u8, s);
}
#endif

static bool arena_begin_temp(arena *arena) {
    bool ret = false;
    arena->temp_depth++;

    arena_temp *temp = push_struct_nz(arena, arena_temp);
    if (temp) {
        slist_stack_push(arena->temp_head, temp);
        temp->block = arena->current_block;
        temp->depth = arena->temp_depth;
        ret = true;
    }

    return ret;
}

static span_u8 arena_block_content(arena_block *block) {
    span_u8 ret = { 0 };
    ret.v       = (u8 *)block     + sizeof(arena_block);
    ret.count   = block->occupied - sizeof(arena_block);
    return ret;
}

static bool arena_ptr_inside_block(arena_block *block, void *ptr) {
    span_u8 content = arena_block_content(block);
    bool ret = (u8 *)ptr >= content.v && (u8 *)ptr < content.v + content.count;
    return ret;
}

static void arena_end_temp(arena *arena) {
    assert(arena->temp_depth > 0 && "Arena temp stack underflow.");

    arena_temp *temp = arena->temp_head;
    if (temp) {
        assert(temp->depth <= arena->temp_depth);
        if (temp->depth == arena->temp_depth) {
            slist_stack_pop(arena->temp_head);

            arena_block *block = temp->block;
            assert(arena_ptr_inside_block(block, temp));
            block->occupied = ix(temp) - ix(block);
            arena->current_block = block;
        }
    }

    arena->temp_depth--;
}

////////////////////////////////////////////////////////////////
//
//
// Dynamic array
//
//
////////////////////////////////////////////////////////////////

static bool array_create_(array *array, ix elemsize, ix initial_capacity, bool init_to_zero) {
    assert(array);
    assert(initial_capacity);
    assert(elemsize);

    // TODO(rune): Error handling
    array->v        = heap_alloc(initial_capacity * elemsize, init_to_zero);
    array->cap      = initial_capacity;
    array->count    = 0;
    return true;
}

static void array_destroy_(array *array) {
    if (array) {
        heap_free(array->v);
        zero(array);
    }
}

static bool array_reserve_(array *array, ix elemsize, ix num, bool init_to_zero) {
    bool ret = false;

    if (array->cap < num) {
        ix new_cap = 0;
        void *new_elems;
        if (array->v == null) {
            new_cap = num;
            new_elems = heap_alloc(new_cap * elemsize, init_to_zero);
        } else {
            new_cap = array->cap;
            while (new_cap < num) {
                new_cap *= 2;
            }

            new_elems = heap_realloc(array->v, new_cap * elemsize, init_to_zero);
        }
        if (new_elems) {
            array->v   = new_elems;
            array->cap = new_cap;

            ret = true;
        }
    } else {
        ret = true;
    }

    return ret;
}

static void *array_add_(array *array, ix elemsize, ix count, bool init_to_zero) {
    void *ret = null;

    if (array_reserve_(array, elemsize, array->count + count, init_to_zero)) {
        ret = (u8 *)array->v + array->count * elemsize;
        array->count += count;
    }

    return ret;
}

static void *array_pop_(array *array, ix elemsize, ix count) {
    void *ret = null;

    if (array->count >= count) {
        array->count  -= count;
        ret = (u8 *)array->v + array->count * elemsize;
    }

    return ret;
}

static bool array_remove_(array *array, ix elemsize, ix idx, ix count) {
    assert(count > 0);

    bool ret = false;

    if (array->count >= idx + count) {
        memmove((u8 *)array->v + elemsize * idx,
                (u8 *)array->v + elemsize * (idx + count),
                elemsize * (array->count - idx - count));

        array->count -= count;
        ret = true;
    }

    return ret;
}

static void *array_insert_(array *array, ix elemsize, ix idx, ix count) {
    assert(count > 0);

    void *ret = null;

    if (array_reserve_(array, elemsize, array->count + count, false)) {
        memmove((u8 *)array->v + elemsize * (idx + count),
                (u8 *)array->v + elemsize * idx,
                (array->count - idx) * elemsize);

        array->count += count;

        ret = (u8 *)array->v + elemsize * idx;
    }

    return ret;
}


static void *array_push_(array *array, ix elemsize, bool init_to_zero) {
    void *ret = array_add_(array, elemsize, 1, init_to_zero);
    return ret;
}

static void array_reset_(array *array, ix elemsize, bool clear_to_zero) {
    array->count = 0;

    if (clear_to_zero) {
        memset(array->v, 0, array->cap * elemsize);
    }
}
