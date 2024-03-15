////////////////////////////////////////////////////////////////
//
//
// Macros
//
//
////////////////////////////////////////////////////////////////

//#define TRACK_ALLOCATIONS

#define kilobytes(x)             ((x) * 1024LL)
#define megabytes(x)             (kilobytes(x) * 1024LL)
#define gigabytes(x)             (megabytes(x) * 1024LL)

#define zero(a)                  (memset((a), 0, sizeof(*(a))))
#define zero_array(a,c)          (memset((a), 0, sizeof(*(a)) * c))

////////////////////////////////////////////////////////////////
//
//
// Heap
//
//
////////////////////////////////////////////////////////////////

// NOTE(rune): MSVC magic memory numbers: https://stackoverflow.com/a/127404
// 0xabababab : Used by Microsoft's HeapAlloc() to mark "no man's land" guard bytes after allocated heap memory
// 0xcccccccc : Used by Microsoft's C++ debugging runtime library to mark uninitialised stack memory
// 0xcdcdcdcd : Used by Microsoft's C++ debugging runtime library to mark uninitialised heap memory
// 0xdddddddd : Used by Microsoft's C++ debugging heap to mark freed heap memory
// 0xfdfdfdfd : Used by Microsoft's C++ debugging heap to mark "no man's land" guard bytes before and after allocated heap memory
// 0xfeeefeee : Used by Microsoft's HeapFree() to mark freed heap memory

static void *   heap_alloc(u64 size, bool init_to_zero);
static void *   heap_realloc(void *p, u64 new_size, bool init_to_zero);
static void     heap_free(void *p);

#define heap_alloc_span(T, count, init_to_zero) ((span(T)) { ._void = heap_alloc_span_(count, sizeof(T), init_to_zero) })
static span(void) heap_alloc_span_(u64 count, u64 elem_size, bool init_to_zero);


// TODO(rune): @Cleanup
static void *os_mem_alloc(u64 size) {
    assert(size % 4096 == 0);
    void *ptr = VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    return ptr;
}

static void *os_mem_reserve(u64 size) {
    assert(size % 4096 == 0);
    void *ptr = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
    return ptr;
}

static void os_mem_commit(void *ptr, u64 size) {
    assert(size % 4096 == 0);
    VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
}

static void os_mem_decommit(void *ptr, u64 size) {
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

static void os_mem_release(void *ptr) {
    VirtualFree(ptr, 0, MEM_RELEASE);
}


////////////////////////////////////////////////////////////////
//
//
// Arena (linear allocator)
//
//
////////////////////////////////////////////////////////////////

typedef struct arena_block arena_block;
struct arena_block {
    u64 capacity;
    arena_block *next;
};

typedef struct arena_snapshot arena_snapshot;
struct arena_snapshot {
    arena_block *block;
    u64 occupied;
};

typedef struct arena_temp arena_temp;
struct arena_temp {
    arena_snapshot snapshot;
    arena_temp  *next;
};

typedef enum arena_growth {
    ARENA_GROWTH_EXPONENTIAL,
    ARENA_GROWTH_LINEAR,
} arena_growth;

typedef struct arena arena;
struct arena {
    arena_snapshot mark;
    arena_block *first_block;
    arena_temp *temp_head;
    arena_growth growth;
};

static void arena_free(arena *arena);
static void arena_reset(arena *arena);

static void *arena_push_size(arena *arena, u64 size, u64 align);
static void *arena_push_size_nz(arena *arena, u64 size, u64 align);

#define arena_push_struct(arena, T)         ((T *)arena_push_size((arena), sizeof(T), alignof(T)))
#define arena_push_struct_nz(arena, T)      ((T *)arena_push_size_nz((arena), sizeof(T), alignof(T)))

#define arena_push_array(arena, T, num)     ((T *)arena_push_size((arena), sizeof(T) * (num), alignof(T)))
#define arena_push_array_nz(arena, T, num)  ((T *)arena_push_size_nz((arena), sizeof(T) * (num), alignof(T)))

static void *arena_copy_size(arena *arena, void *src, u64 size, u64 align);
static str   arena_copy_str(arena *arena, str s);

static void arena_begin_temp(arena *arena);
static void arena_end_temp(arena *arena);

#define arena_temp(arena)  defer_scope(arena_begin_temp(arena), arena_end_temp(arena))

////////////////////////////////////////////////////////////////
//
//
// Generic dynamic array
//
//
////////////////////////////////////////////////////////////////

#define darray(T)                                                array_##T
#define array_size(array)                                        ((array)->count * array_elem_size(array))
#define array_size_cap(array)                                    ((array)->cap * array_elem_size(array))
#define array_is_empty(array)                                    ((array)->count == 0)
#define array_first(array)                                       (*((array).count > 0 ? &(array).v[0]               : null))
#define array_first_or_default(array, default)                   ( ((array).count > 0 ?  (array).v[0]               : default))
#define array_last(array)                                        (*((array).count > 0 ? &(array).v[(array).count - 1] : null))
#define array_last_or_default(array, default)                    ( ((array).count > 0 ?  (array).v[(array).count - 1] : default))
#define array_front(array)                                       ((array).v)
#define array_back(array)                                        ((array).v + (array).count)
#define array_elem_size(array)                                   (sizeof(*(array)->v))
#define array_get(array, idx)                                    ((array)->v + (idx))

#define darray_create(array, initial_capacity, init_to_zero)      darray_create_     (&(array)->_void, array_elem_size(array), initial_capacity, init_to_zero)
#define darray_destroy(array)                                     darray_destroy_    (&(array)->_void)
#define darray_reserve(array, count, init_to_zero)                darray_reserve_    (&(array)->_void, array_elem_size(array), count, init_to_zero)
#define darray_add(array, count, init_to_zero)                    darray_add_        (&(array)->_void, array_elem_size(array), count, init_to_zero)
#define darray_pop(array, count)                                  darray_pop_        (&(array)->_void, array_elem_size(array), count)
#define darray_remove(array, idx, count)                          darray_remove_     (&(array)->_void, array_elem_size(array), idx, count)
#define darray_insert(array, idx, count)                          darray_insert_     (&(array)->_void, array_elem_size(array), idx, count)
#define darray_reset(array, clear_to_zero)                        darray_reset_      (&(array)->_void, array_elem_size(array), clear_to_zero)
#define darray_put(array, val)                                    (darray_add(array, 1, false) ? ((array)->v[(array)->count - 1] = (val), &(array)->v[(array)->count - 1]) : null)
#define array_idx_of(array, ptr)                                  ((ptr) - (array).v)

#define array_as_span(T, array) ((span(T)) { .v = (array).v, .count = (array).count })

#define darray_struct(T)      struct { T *v; u64 count, cap; }
#define typedef_darray(T)     typedef union { darray_struct(T); array _void; } darray(T)

typedef darray_struct(void) array;

typedef_darray(u8);
typedef_darray(u16);
typedef_darray(u32);
typedef_darray(u64);
typedef_darray(i8);
typedef_darray(i16);
typedef_darray(i32);
typedef_darray(i64);

static bool darray_create_(array *array, u64 elem_size, u64 initial_cap, bool init_to_zero);
static void darray_destroy_(array *array);
static bool darray_reserve_(array *array, u64 elem_size, u64 reserve_count, bool init_to_zero);
static void *darray_add_(array *array, u64 elem_size, u64 add_count, bool init_to_zero);
static void *darray_pop_(array *array, u64 elem_size, u64 pop_count);
static bool darray_remove_(array *array, u64 elem_size, u64 idx, u64 remove_count);
static void *darray_insert_(array *array, u64 elem_size, u64 idx, u64 insert_count);
static void *array_push_(array *array, u64 elem_size, bool init_to_zero);
static void darray_reset_(array *array, u64 elem_size, bool clear_to_zero);
