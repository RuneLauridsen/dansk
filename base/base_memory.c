////////////////////////////////////////////////////////////////
//
//
// Allocation tracking
//
//
////////////////////////////////////////////////////////////////

#pragma warning ( push )
#pragma warning ( disable : 4255 ) // C4255: no function prototype given : converting '()' to '(void)'
#ifdef TRACK_ALLOCATIONS
#include "thirdparty/idk.h"
#include "thirdparty/idk.c"
#endif
#pragma warning ( pop )

////////////////////////////////////////////////////////////////
//
//
// Heap
//
//
////////////////////////////////////////////////////////////////

static void *heap_alloc(u64 size, bool init_to_zero) {
    void *ret = HeapAlloc(GetProcessHeap(), init_to_zero ? HEAP_ZERO_MEMORY : 0, size);
    assert(ret);
#ifdef TRACK_ALLOCATIONS
    idk_track_alloc(IDK_LOCATION, ret, size);
#endif
    return ret;
}

static void *heap_realloc(void *p, u64 size, bool init_to_zero) {
    void *ret = HeapReAlloc(GetProcessHeap(), init_to_zero ? HEAP_ZERO_MEMORY : 0, p, size);
    assert(ret);
#ifdef TRACK_ALLOCATIONS
    idk_track_realloc(IDK_LOCATION, p, ret, size);
#endif
    return ret;
}

static void heap_free(void *p) {
    if (p) {
        HeapFree(GetProcessHeap(), 0, p);
#ifdef TRACK_ALLOCATIONS
        idk_track_free(IDK_LOCATION, p);
#endif
    }
}

static span(void) heap_alloc_span_(u64 count, u64 elem_size, bool init_to_zero) {
    void_span ret = { 0 };
    ret.v = heap_alloc(count * elem_size, init_to_zero);
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

static void arena_free(arena *arena) {
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

static void arena_reset(arena *arena) {
    arena->mark.block = null;
    arena->mark.occupied = 0;
}

static void *arena_push_size(arena *arena, u64 size, u64 align) {
    void *ret = arena_push_size_nz(arena, size, align);
    memset(ret, 0, size);
    return ret;
}

static void *arena_push_size_nz(arena *arena, u64 size, u64 align) {
    // Push empty space to ensure alignment.
    if (align == 0)  align = 1;
    arena->mark.occupied = u64_align_to_pow2(arena->mark.occupied, align);

    // Do we need a new block?
    arena_block *curr_block = arena->mark.block;
    if (curr_block == null || arena->mark.occupied + size > curr_block->capacity) {
        // Use an existing block if available.
        u64 aligned_header_size = u64_align_to_pow2(sizeof(arena_block), align);
        arena_block *next_block = curr_block ? curr_block->next : arena->first_block;
        while (next_block) {
            if (aligned_header_size + size <= next_block->capacity) {
                break;
            }

            curr_block = next_block;
            next_block = next_block->next;
        }

        // No existing block found -> allocate new block
        if (next_block == null) {
            u64 next_capacity = 4096;
            if (curr_block) {
                if (arena->growth == ARENA_GROWTH_EXPONENTIAL) next_capacity = curr_block->capacity * 2;
                if (arena->growth == ARENA_GROWTH_LINEAR)      next_capacity = curr_block->capacity;
            }

            next_capacity = max(next_capacity, aligned_header_size + size);
            next_capacity = u64_round_up_to_pow2(next_capacity);

            next_block = (arena_block *)heap_alloc(next_capacity, false);
            next_block->capacity = next_capacity;
            next_block->next     = null;

            if (arena->first_block == null) arena->first_block = next_block;
            if (curr_block)                 curr_block->next   = next_block;
        }

        // Reset mark since we moved to a new block.
        arena->mark.occupied = aligned_header_size;
        arena->mark.block    = next_block;
    }

    // Push size.
    assert(arena->mark.occupied + size <= arena->mark.block->capacity);
    void *ret = (u8 *)arena->mark.block + arena->mark.occupied;
    arena->mark.occupied += size;

    return ret;
}

static void *arena_copy_size(arena *arena, void *src, u64 size, u64 align) {
    void *dst = arena_push_size(arena, size, align);
    memcpy(dst, src, size);
    return dst;
}

static str arena_copy_str(arena *arena, str s) {
    str ret = { 0 };
    ret.v = (u8 *)arena_push_size_nz(arena, s.len + 1, 0);
    ret.len = s.len;

    memcpy(ret.v, s.v, s.len);
    ret.v[ret.len] = '\0';

    return ret;
}

static arena_snapshot arena_take_snapshot(arena *arena) {
    return arena->mark;
}

static void arena_restore_snapshot(arena *arena, arena_snapshot snapshot) {
    arena->mark = snapshot;
}

static void arena_begin_temp(arena *arena) {
    arena_snapshot snapshot = arena_take_snapshot(arena);
    arena_temp *temp = arena_push_struct_nz(arena, arena_temp);
    temp->snapshot = snapshot;
    slist_stack_push(&arena->temp_head, temp);
}

static void arena_end_temp(arena *arena) {
    assert(arena->temp_head && "Arena temp stack underflow.");
    arena_snapshot snapshot = arena->temp_head->snapshot;
    slist_stack_pop(&arena->temp_head);
    arena_restore_snapshot(arena, snapshot);
}

////////////////////////////////////////////////////////////////
//
//
// Dynamic array
//
//
////////////////////////////////////////////////////////////////

static bool darray_create_(array *array, u64 elem_size, u64 initial_cap, bool init_to_zero) {
    assert(array);
    assert(initial_cap);
    assert(elem_size);

    // TODO(rune): Error handling
    array->v        = heap_alloc(initial_cap * elem_size, init_to_zero);
    array->cap      = initial_cap;
    array->count    = 0;
    return true;
}

static void darray_destroy_(array *array) {
    if (array) {
        heap_free(array->v);
        zero(array);
    }
}

static bool darray_reserve_(array *array, u64 elem_size, u64 reserve_count, bool init_to_zero) {
    bool ret = false;

    if (array->cap < reserve_count) {
        u64 new_cap = 0;
        void *new_elems;
        if (array->v == null) {
            new_cap = reserve_count;
            new_elems = heap_alloc(new_cap * elem_size, init_to_zero);
        } else {
            new_cap = array->cap;
            while (new_cap < reserve_count) {
                new_cap *= 2;
            }

            new_elems = heap_realloc(array->v, new_cap * elem_size, init_to_zero);
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

static void *darray_add_(array *array, u64 elem_size, u64 add_count, bool init_to_zero) {
    void *ret = null;

    if (darray_reserve_(array, elem_size, array->count + add_count, init_to_zero)) {
        ret = (u8 *)array->v + array->count * elem_size;
        array->count += add_count;
    }

    return ret;
}

static void *darray_pop_(array *array, u64 elem_size, u64 pop_count) {
    void *ret = null;

    if (array->count >= pop_count) {
        array->count  -= pop_count;
        ret = (u8 *)array->v + array->count * elem_size;
    }

    return ret;
}

static bool darray_remove_(array *array, u64 elem_size, u64 idx, u64 remove_count) {
    assert(remove_count > 0);

    bool ret = false;

    if (array->count >= idx + remove_count) {
        memmove((u8 *)array->v + elem_size * idx,
                (u8 *)array->v + elem_size * (idx + remove_count),
                elem_size * (array->count - idx - remove_count));

        array->count -= remove_count;
        ret = true;
    }

    return ret;
}

static void *darray_insert_(array *array, u64 elem_size, u64 idx, u64 insert_count) {
    assert(insert_count > 0);

    void *ret = null;

    if (darray_reserve_(array, elem_size, array->count + insert_count, false)) {
        memmove((u8 *)array->v + elem_size * (idx + insert_count),
                (u8 *)array->v + elem_size * idx,
                (array->count - idx) * elem_size);

        array->count += insert_count;

        ret = (u8 *)array->v + elem_size * idx;
    }

    return ret;
}


static void *array_push_(array *array, u64 elem_size, bool init_to_zero) {
    void *ret = darray_add_(array, elem_size, 1, init_to_zero);
    return ret;
}

static void darray_reset_(array *array, u64 elem_size, bool clear_to_zero) {
    array->count = 0;

    if (clear_to_zero) {
        memset(array->v, 0, array->cap * elem_size);
    }
}
