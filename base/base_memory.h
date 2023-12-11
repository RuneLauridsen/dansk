////////////////////////////////////////////////////////////////
//
//
// Macros
//
//
////////////////////////////////////////////////////////////////

// #define TRACK_ALLOCATIONS

#define coalesce(a,b)            ((a)?(a):(b))
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

static void *   heap_alloc(ix size, bool init_to_zero);
static void *   heap_realloc(void *p, ix new_size, bool init_to_zero);
static void     heap_free(void *p);

#define heap_alloc_span(T, count, init_to_zero) ((span(T)) { ._void = heap_alloc_span_(count, sizeof(T), init_to_zero) })
static span(void) heap_alloc_span_(ix count, ix elemsize, bool init_to_zero);

////////////////////////////////////////////////////////////////
//
//
// Linked lists
//
//
////////////////////////////////////////////////////////////////

#define SLQUEUE_JOIN(fa,la,fb,lb,next)                                                \
    if ((fa) == null) { assert(la == null); (fa) = (fb); (la) = (lb); }               \
    else              { assert((la)->next == null); (la)->next = (fb); (la) = (lb); } \

#define SLQUEUE_PUSH(f,l,next,n)                        \
    SLQUEUE_JOIN(f,l,n,n,next)

#define SLQUEUE_POP(f,l,next)                           \
    if ((f) == (l)) { (f) = (l) = null; }               \
    else            { (f) = (f)->next; }                \

#define SLSTACK_PUSH(f,next,n)                          \
    (n)->next = (f);                                    \
    (f) = (n);

#define SLSTACK_POP(f,next)                             \
    if((f)) { (f) = (f)->next; }

#define DLIST_PUSH_BACK(f,l,next,prev,n)                \
    if ((f) == null) {                                  \
        assert((l) == null);                            \
        (f) = n;                                        \
        (l) = n;                                        \
    } else {                                            \
        assert((f)->prev == null);                      \
        assert((l)->next == null);                      \
        (l)->next = (n);                                \
        (n)->prev = (l);                                \
        (l) = (n);                                      \
    }

#define DLIST_PUSH_FRONT(f,l,next,prev,n)               \
    DLIST_PUSH_BACK(l,f,prev,next,n)                    \

#define DLIST_REMOVE(f,l,next,prev,n)                   \
    if((f) == (n)) {                                    \
        if ((f)->next) {                                \
            (f) = (f)->next;                            \
            (f)->prev = null;                           \
        } else {                                        \
            (f) = null;                                 \
        }                                               \
    } else {                                            \
        (n)->prev->next = (n)->next;                    \
    }                                                   \
                                                        \
    if ((l) == (n)) {                                   \
        if ((l)->prev) {                                \
            (l) = (l)->prev;                            \
            (l)->next = null;                           \
        } else {                                        \
            (l) = null;                                 \
        }                                               \
    } else {                                            \
        (n)->next->prev = (n)->prev;                    \
    }                                                   \
                                                        \
    (n)->next = null;                                   \
    (n)->prev = null;

#define DLIST_INSERT(f, l, next, prev, after, n)        \
    if ((after) == (l)) {                               \
        DLIST_PUSH_BACK(f, l, next, prev, n);           \
    } else {                                            \
        (n)->next = (after)->next;                      \
        (n)->prev = (after);                            \
        (after)->next->prev = n;                        \
        (after)->next = n;                              \
    }

#define DLIST_STACK_PUSH(head, next, prev, n)           \
    if (head) (head)->next = (n);                       \
    (n)->prev = (head);                                 \
    (head) = (n);

#define DLIST_STACK_POP(head, next, prev)               \
    (head) = (head)->prev;                              \
    if (head) (head)->next = null;

#define DLIST_STACK_REMOVE(head, next, prev, n)             \
    if ((n)->next) (n)->next->prev = (n)->prev;             \
    if ((n)->prev) (n)->prev->next = (n)->next;             \
    if ((n) == (head)) DLIST_STACK_POP(head, next, prev)    \
    (n)->next = null;                                       \
    (n)->prev = null;

// TODO(rune): Better naming than slist, slist_stack, dlist and dlist_stack. Depending on the combination
// of 'first-last' <> 'head' and 'next-prev' <> 'next' pointers, we end up with have 4 kinds of linked list.
//
//    slist_queue:    first-last      next
//    slist_stack:    head            next
//    dlist:          first-last      next-prev
//    dlist_stack:    head            next-prev
//

#define slist_queue_join(a,b)          SLQUEUE_JOIN((a)->first, (a)->last, (b)->first, (b)->last,next)
#define slist_queue_push(list,n)       SLQUEUE_PUSH((list)->first, (list)->last, next, n)
#define slist_queue_pop(list)          SLQUEUE_POP((list)->first, (list)->last, next)
#define slist_add(list, node)          slist_queue_push(list, node) // NOTE(rune): Just an alias for slist_queue_push.

#define slist_stack_push(head, n)      SLSTACK_PUSH(head, next, n)
#define slist_stack_pop(head)          SLSTACK_POP(head, next)

#define dlist_push_front(list, node)   DLIST_PUSH_FRONT((list)->first, (list)->last, next, prev, node)
#define dlist_push_back(list, node)    DLIST_PUSH_BACK ((list)->first, (list)->last, next, prev, node)
#define dlist_insert_after(list, node, after)   DLIST_INSERT   ((list)->first, (list)->last, next, prev, after, node)
#define dlist_insert_before(list, node, before) DLIST_INSERT   ((list)->last, (list)->first, prev, next, before, node)
#define dlist_remove(list, node)       DLIST_REMOVE    ((list)->first, (list)->last, next, prev, node)
#define dlist_add(list, node)          dlist_push_back(list, node) // NOTE(rune): Just an alias for dlist_push_back.

#define dlist_stack_push(head, node)   DLIST_STACK_PUSH(head, next, prev, node)
#define dlist_stack_pop(head)          DLIST_STACK_POP(head, next, prev)
#define dlist_stack_remove(head, node) DLIST_STACK_REMOVE(head, next, prev, node)

////////////////////////////////////////////////////////////////
//
//
// Arena (linear allocator)
//
//
////////////////////////////////////////////////////////////////

typedef enum arena_kind {
    ARENA_KIND_NO_CHAIN,
    ARENA_KIND_LINEAR_CHAIN,
    ARENA_KIND_EXPONENTIAL_CHAIN
} arena_kind;

typedef struct arena_block arena_block;
struct arena_block {
    ix occupied;
    ix capacity;
    arena_block *next;
    arena_block *prev;
};

typedef struct arena_temp arena_temp;
struct arena_temp {
    arena_block *block;
    arena_temp  *next;

    // NOTE(rune): In case there wasn't enough space to allocate an arena_temp in
    // arena_begin_temp, we still force the user to call arena_end_temp, and use
    // arena.temp_depth to determine if we are popping the right arena_temp.
    u32 depth;
};

typedef struct arena arena;
struct arena {
    arena_block *first_block;
    arena_block *current_block;

    arena_temp *temp_head;
    u32         temp_depth;

    arena_kind kind;
};

static void arena_init(arena *arena, void *memory, ix size);
static bool arena_create(arena *arena, ix size, bool init_to_zero, arena_kind kind);
static void free_arena(arena *arena);
static void arena_reset(arena *arena);

static void *push_size(arena *arena, ix size);
static void *push_size_nz(arena *arena, ix size);
static void *push_padded_size(arena *arena, ix size, ix pad);
static void *push_padded_size_nz(arena *arena, ix size, ix pad);

#define push_struct(arena, ...)       ((__VA_ARGS__ *)push_size((arena), sizeof(__VA_ARGS__)))
#define push_struct_nz(arena, ...)    ((__VA_ARGS__ *)push_size((arena), sizeof(__VA_ARGS__)))

#define push_span(arena, T, num)      ((span(T)) { ._void = push_span_(arena, num, sizeof(T)) })
#define push_span_nz(arena, T, num)   ((span(T)) { ._void = push_span_nz_(arena, num, sizeof(T)) })
#define copy_span(arena, T, src)      ((span(T)) { ._void = copy_span_(arena, (src)._void, sizeof(T)) })

static str copy_str(arena *arena, str s);
static str copy_str_null_terminate(arena *arena, str s);

static bool arena_begin_temp(arena *arena);
static void arena_end_temp(arena *arena);

#define arena_temp(arena)  defer_scope(arena_begin_temp(arena), arena_end_temp(arena))

////////////////////////////////////////////////////////////////
//
//
// Generic dynamic array
//
//
////////////////////////////////////////////////////////////////

#define array(T)                                                 array_##T
#define array_size(array)                                        ((array)->count * array_elemsize(array))
#define array_size_cap(array)                                    ((array)->cap * array_elemsize(array))
#define array_is_empty(array)                                    ((array)->count == 0)
#define array_first(array)                                       (*((array).count > 0 ? &(array).v[0]               : null))
#define array_first_or_default(array, default)                   ( ((array).count > 0 ?  (array).v[0]               : default))
#define array_last(array)                                        (*((array).count > 0 ? &(array).v[(array).count - 1] : null))
#define array_last_or_default(array, default)                    ( ((array).count > 0 ?  (array).v[(array).count - 1] : default))
#define array_front(array)                                       ((array).v)
#define array_back(array)                                        ((array).v + (array).count)
#define array_elemsize(array)                                    (sizeof(*(array)->v))
#define array_get(array, idx)                                    ((array)->v + (idx)) // TODO(rune): Bounds check.

#define array_create(array, initial_capacity, init_to_zero)      array_create_     (&(array)->_void, array_elemsize(array), initial_capacity, init_to_zero)
#define array_destroy(array)                                     array_destroy_    (&(array)->_void)
#define array_reserve(array, count, init_to_zero)                array_reserve_    (&(array)->_void, array_elemsize(array), count, init_to_zero)
#define array_add(array, count, init_to_zero)                    array_add_        (&(array)->_void, array_elemsize(array), count, init_to_zero)
#define array_pop(array, count)                                  array_pop_        (&(array)->_void, array_elemsize(array), count)
#define array_remove(array, idx, count)                          array_remove_     (&(array)->_void, array_elemsize(array), idx, count)
#define array_insert(array, idx, count)                          array_insert_     (&(array)->_void, array_elemsize(array), idx, count)
#define array_reset(array, clear_to_zero)                        array_reset_      (&(array)->_void, array_elemsize(array), clear_to_zero)
#define array_put(array, val)                                    (array_add(array, 1, false) ? ((array)->v[(array)->count - 1] = (val), &(array)->v[(array)->count - 1]) : null)
#define array_idx_of(array, ptr)                                 ((ptr) - (array).v)

#define array_as_span(T, array) ((span(T)) { .v = (array).v, .count = (array).count })

#define array_struct(T)      struct { T *v; ix count, cap; }
#define typedef_array(T)     typedef union { array_struct(T); array _void; } array(T)

typedef array_struct(void) array;

typedef_array(u8);
typedef_array(u16);
typedef_array(u32);
typedef_array(u64);
typedef_array(i8);
typedef_array(i16);
typedef_array(i32);
typedef_array(i64);

static bool array_create_(array *array, ix elemsize, ix initial_capacity, bool init_to_zero);
static void array_destroy_(array *array);
static bool array_reserve_(array *array, ix elemsize, ix num, bool init_to_zero);
static void *array_add_(array *array, ix elemsize, ix num, bool init_to_zero);
static void *array_pop_(array *array, ix elemsize, ix num);
static bool array_remove_(array *array, ix elemsize, ix idx, ix num);
static void *array_push_(array *array, ix elemsize, bool init_to_zero);
static void array_reset_(array *array, ix elemsize, bool clear_to_zero);
