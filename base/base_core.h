////////////////////////////////////////////////////////////////
//
//
// Warnings
//
//
////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4201 ) // C4201: nonstandard extension used: nameless struct/union
#pragma warning ( disable : 4127 ) // C4127: conditional expression is constant
#pragma warning ( disable : 4702 ) // C4702: unreachable code
#pragma warning ( disable : 4505 ) // C4505: unreferenced function with internal linkage has been removed
#pragma warning ( 4       : 4255 ) // C4255: no function prototype given : converting '()' to '(void)'

#if 1 // Disable warning for unused locals/parameters?
#pragma warning ( disable : 4189 ) // C4189: local variable is initialized but not referenced
#pragma warning ( disable : 4101 ) // C4101: unreferenced local variable
#pragma warning ( disable : 4100 ) // C4100: unreferenced formal parameter
#endif

////////////////////////////////////////////////////////////////
//
//
// Scalars
//
//
////////////////////////////////////////////////////////////////

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef u8          b8;
typedef u16         b16;
typedef u32         b32;
typedef u64         b64;
typedef float       f32;
typedef double      f64;

#define U8_MIN  (0)
#define U16_MIN (0)
#define U32_MIN (0)
#define U64_MIN (0)

#define U8_MAX  (0xff)
#define U16_MAX (0xffff)
#define U32_MAX (0xffff'ffff)
#define U64_MAX (0xffff'ffff'ffff'ffff)

////////////////////////////////////////////////////////////////
//
//
// Utility macros
//
//
////////////////////////////////////////////////////////////////

#define countof(a)          (sizeof(a) / sizeof(*(a)))
#define lengthof(a)         (sizeof(a) - 1)
#define memberof(T,m)       (((T *)0)->m)

#define fallthrough
#define outparam

#pragma section(".readonly", read)
#define readonly __declspec(allocate(".readonly"))

#define unused(...)     (void)(__VA_ARGS__)

#ifndef null
#define null 0
#endif

// NOTE: Cast to _Bool so that anyof(false) and anyof(true) works properly.
#ifndef __cplusplus
#undef false
#define false ((bool)(0))
#undef true
#define true ((bool)(1))
#endif

#ifndef min
#define min(a,b)        ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b)        ((a)>(b)?(a):(b))
#endif
#define clamp(a,l,h)                (max(min(a,h),l))
#define clamp01(a)                  (clamp(a,0,1))
#define clamp_bot(a,l)              (max((a), (l)))
#define clamp_top(a,h)              (min((a), (h)))
#define clamp_to_range(a,r)         (clamp((a), (r).min, (r).max))

#define min_assign(a,b)             (*(a) = min(*(a),b))
#define max_assign(a,b)             (*(a) = max(*(a),b))
#define clamp_assign(a,l,h)         (*(a) = clamp(*(a),l,h))
#define clamp01_assign(a)           (*(a) = clamp01(*(a)))
#define clamp_bot_assign(a,l)       (*(a) = clamp_bot(*(a),l))
#define clamp_top_assign(a,h)       (*(a) = clamp_top(*(a),h))
#define clamp_to_range_assign(a,r)  (*(a) = clamp_to_range(*(a),r))

#define swap(T, a, b)           do { T __swap_temp = *(a); *(a) = *(b); *(b) = __swap_temp; } while(0);

#define coalesce(a,b)            ((a)?(a):(b))

#ifdef __cplusplus
#define lit(T) T
#define extern_c extern "C"
#else
#define lit(T) (T)
#define extern_c extern
#endif

#define full_switch_begin _Pragma("warning ( push )") _Pragma("warning ( error : 4061)") // C4061: enumerator in switch of enum is not explicitly handled by a case label.
#define full_switch_end   _Pragma("warning ( pop )")

#define cast(T, a)      ((T)(a))
#define cast_i8(a)      ((i8)(a))
#define cast_i16(a)     ((i16)(a))
#define cast_i32(a)     ((i32)(a))
#define cast_i64(a)     ((i64)(a))
#define cast_u8(a)      ((u8)(a))
#define cast_u16(a)     ((u16)(a))
#define cast_u32(a)     ((u32)(a))
#define cast_u64(a)     ((u64)(a))
#define cast_b8(a)      ((b8)(a))
#define cast_b16(a)     ((b16)(a))
#define cast_b32(a)     ((b32)(a))
#define cast_b64(a)     ((b64)(a))
#define cast_f32(a)     ((f32)(a))
#define cast_f64(a)     ((f64)(a))

#define PACK_U8X8(u0, u1, u2, u3)  ((cast_u32(u0) <<  0) | \
                                    (cast_u32(u1) <<  8) | \
                                    (cast_u32(u2) << 16) | \
                                    (cast_u32(u3) << 24))

#define rgba(r,g,b,a)   (PACK_U8X8(r, g, b, a))
#define rgb(r,g,b)      (PACK_U8X8(r, g, b, 255))

#define defer_scope__(begin, end, i) for(int _defer##i##_ = ((begin), 0); _defer##i##_ == 0; _defer##i##_ += 1, (end))
#define defer_scope_(begin, end, i)  defer_scope__(begin, end, i)
#define defer_scope(begin, end)      defer_scope_(begin, end, __LINE__)

#define sign(a)     ((a) < 0 ? -1 : 1)

#define concat_(name, T) name##T
#define concat(name, T)  concat_(name, T)

#define stringify_(a) #a
#define stringify(a) stringify_(a)
#define loc() (__FILE__ ":" stringify(__LINE__))

#define for_range(T, it, start, end)        for(T it = start; it < end; it++)
#define for_n(T, it, end)                   for(T it = 0; it < end; it++)
#define for_count(T, it, items, count)      for (T *it = (items); it < (items) + (count); it++)
#define for_span(T, it, span)               for_count(T, it, (span).v, (span).count)
#define for_array(T, it, array)             for_count(T, it, (array), countof(array))
#define for_list(T, it, list)               for (T *it = (list).first; it; it = it->next)
#define for_list_rev(T, it, list)           for (T *it = (list).last; it; it = it->prev)
#define loop(n)                             for (u64 _i_ = 0; _i_ < n; _i_++)

#define spanof(a)  (make_span(u8, (u8 *)&(a), sizeof(a)))

#ifndef YO_PROFILE_BEGIN
#define YO_PROFILE_BEGIN(...)
#endif

#ifndef YO_PROFILE_END
#define YO_PROFILE_END(...)
#endif

////////////////////////////////////////////////////////////////
//
//
// Assertions
//
//
////////////////////////////////////////////////////////////////

#define debug_break() __debugbreak();
#define nop() __nop();

#undef assert
#if NDEBUG
#define assert(expression)
#else
#define assert(expression)                                                               \
    do {                                                                                 \
        if(!(expression)) {                                                              \
            printf("================================================================\n"  \
                   "                        ASSERTION FAILED                        \n"  \
                   "Assertion:                                                      \n"  \
                   "    assert(%s);                                                 \n"  \
                   "                                                                \n"  \
                   "Location:                                                       \n"  \
                   "    %s:%i                                                       \n"  \
                   "                                                                \n"  \
                   "Stacktrace:                                                     \n"  \
                   "    %s                                                          \n"  \
                   "                                                                \n"  \
                   "================================================================\n", \
                   #expression,                                                          \
                   __FILE__, __LINE__,                                                   \
                   "TODO");                                                              \
                                                                                         \
            debug_break();                                                               \
        }                                                                                \
    } while(0)
#endif

#define assert_array_bounds(idx, array) (assert_bounds_((idx), countof(array)))
#define assert_bounds(idx, span) (assert_bounds_((idx), (span).count))

static inline void assert_bounds_(u64 idx, u64 max) {
    assert(idx >= 0);
    assert(idx < max);
}

////////////////////////////////////////////////////////////////
//
//
// Basic types
//
//
////////////////////////////////////////////////////////////////

typedef struct { void *v; union { u64 count, len; }; } void_span;

#define span_struct(T)              \
union {                             \
    struct {                        \
        T *v;                       \
        union { u64 count, len; };  \
    };                              \
    struct {                        \
        void_span _void;            \
    };                              \
}

#define result_struct(T) struct {       \
    T v;                                \
    union { bool has_value, found; };   \
}

#define typedef_span(T)   typedef span_struct(T) span(T)
#define typedef_result(T) typedef result_struct(T) result(T)

#define span(T)   T##_span
#define result(T) T##_result
#define array(T)  struct { T *v; u64 count; }

typedef_span(i8);
typedef_span(i16);
typedef_span(i32);
typedef_span(i64);
typedef_span(u8);
typedef_span(u16);
typedef_span(u32);
typedef_span(u64);
typedef_span(f32);
typedef_span(f64);

typedef_result(i8);
typedef_result(i16);
typedef_result(i32);
typedef_result(i64);
typedef_result(u8);
typedef_result(u16);
typedef_result(u32);
typedef_result(u64);
typedef_result(f32);
typedef_result(f64);

#define make_span(T, v, count)           (lit(span(T)) { (v), (count) })

typedef u8_span span;
typedef u8_span str;
typedef u16_span wstr;

typedef_span(str);
typedef_span(wstr);

static inline str make_str(u8 *v, u64 count) { return make_span(u8, v, count); }
static inline wstr make_wstr(u16 *v, u64 count) { return make_span(u16, v, count); }
static inline span wstr_as_span(wstr w) { return make_span(u8, (u8 *)w.v, w.count * 2); }

#define span_cast(a, T) (make_span(T, (T *)(a).v, ((a).len * sizeof((a).v[0])) / sizeof(T)))

#define list(T)                        struct { T *first, *last; }
#define slist_node(T)                  struct { T v, *next; }
#define dlist_node(T)                  struct { T v, *next, *prev; }

#define span_literal(T, ...)          (make_span(T, ((T[]) { __VA_ARGS__ }), NUM_ARGUMENTS(__VA_ARGS__)))

// NOTE(rune): Need at seperate macro for compile-time constant strings, because C is dumb.
#define STR(text)                      { .v = (u8 *)(text), .len = lengthof(text)}
#define str(text)                      make_span(u8, (u8 *)(text), lengthof(text))
#define spanof_array(T, array)         make_span(T, array, countof(array))

static inline u64 ptr_diff(void *high, void *low) {
    return (u8 *)high - (u8 *)low;
}

static inline u64 u64_from_ptr(void *ptr) {
    return (u64)ptr;
}

////////////////////////////////////////////////////////////////
//
//
// Basic enums
//
//
////////////////////////////////////////////////////////////////

typedef enum side { SIDE_MIN, SIDE_MAX } side;

typedef enum axis2 { AXIS2_X, AXIS2_Y } axis2;
typedef enum axis3 { AXIS3_X, AXIS3_Y, AXIS3_Z } axis3;
typedef enum axis4 { AXIS4_X, AXIS4_Y, AXIS4_Z, AXIS4_W } axis4;

typedef enum dir { DIR_FORWARD = 1, DIR_BACKWARD = -1, } dir;
typedef enum dir2 { DIR2_LEFT, DIR2_TOP, DIR2_RIGHT, DIR2_BOT } dir2;
typedef enum dir3 { DIR3_LEFT, DIR3_TOP, DIR3_RIGHT, DIR3_BOT, DIR3_UP, DIR3_DOWN } dir3;

typedef enum corner { CORNER_TOP_LEFT, CORNER_BOTTOM_LEFT, CORNER_TOP_RIGHT, CORNER_BOTTOM_RIGHT } corner;

typedef enum move_by {
    MOVE_BY_CHAR = 1,
    MOVE_BY_WORD,
    MOVE_BY_SUBWORD,
    MOVE_BY_LINE,
    MOVE_BY_PARAGRAPH,
    MOVE_BY_PAGE,
    MOVE_BY_HOME_END,
    MOVE_BY_DOCUMENT,
} move_by;

////////////////////////////////////////////////////////////////
//
//
// Evil macros
//
//
////////////////////////////////////////////////////////////////

#define NUM_ARGUMENTS_(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,n,...) n
#define NUM_ARGUMENTS(...) NUM_ARGUMENTS_(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define OVERLOAD__(name, n) name##n
#define OVERLOAD_(name, n)  OVERLOAD__(name, n)
#define OVERLOAD(name, ...) OVERLOAD_(name, NUM_ARGUMENTS(__VA_ARGS__))(__VA_ARGS__)

#define VA_WRAP_1(wrap)
#define VA_WRAP_2(wrap,a1)                                                      wrap(a1)
#define VA_WRAP_3(wrap,a1,a2)                                                   wrap(a1),wrap(a2)
#define VA_WRAP_4(wrap,a1,a2,a3)                                                wrap(a1),wrap(a2),wrap(a3)
#define VA_WRAP_5(wrap,a1,a2,a3,a4)                                             wrap(a1),wrap(a2),wrap(a3),wrap(a4)
#define VA_WRAP_6(wrap,a1,a2,a3,a4,a5)                                          wrap(a1),wrap(a2),wrap(a3),wrap(a4),wrap(a5)
#define VA_WRAP_7(wrap,a1,a2,a3,a4,a5,a6)                                       wrap(a1),wrap(a2),wrap(a3),wrap(a4),wrap(a5),wrap(a6)
#define VA_WRAP_8(wrap,a1,a2,a3,a4,a5,a6,a7)                                    wrap(a1),wrap(a2),wrap(a3),wrap(a4),wrap(a5),wrap(a6),wrap(a7)
#define VA_WRAP_9(wrap,a1,a2,a3,a4,a5,a6,a7,a8)                                 wrap(a1),wrap(a2),wrap(a3),wrap(a4),wrap(a5),wrap(a6),wrap(a7),wrap(a8)
#define VA_WRAP_10(wrap,a1,a2,a3,a4,a5,a6,a7,a8,a9)                             wrap(a1),wrap(a2),wrap(a3),wrap(a4),wrap(a5),wrap(a6),wrap(a7),wrap(a8),wrap(a9)
#define VA_WRAP_11(wrap,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10)                         wrap(a1),wrap(a2),wrap(a3),wrap(a4),wrap(a5),wrap(a6),wrap(a7),wrap(a8),wrap(a9),wrap(a10)
#define VA_WRAP_12(wrap,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11)                     wrap(a1),wrap(a2),wrap(a3),wrap(a4),wrap(a5),wrap(a6),wrap(a7),wrap(a8),wrap(a9),wrap(a10),wrap(a11)
#define VA_WRAP_13(wrap,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12)                 wrap(a1),wrap(a2),wrap(a3),wrap(a4),wrap(a5),wrap(a6),wrap(a7),wrap(a8),wrap(a9),wrap(a10),wrap(a11),wrap(a12)
#define VA_WRAP_14(wrap,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13)             wrap(a1),wrap(a2),wrap(a3),wrap(a4),wrap(a5),wrap(a6),wrap(a7),wrap(a8),wrap(a9),wrap(a10),wrap(a11),wrap(a12),wrap(a13)
#define VA_WRAP_15(wrap,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14)         wrap(a1),wrap(a2),wrap(a3),wrap(a4),wrap(a5),wrap(a6),wrap(a7),wrap(a8),wrap(a9),wrap(a10),wrap(a11),wrap(a12),wrap(a13),wrap(a14)
#define VA_WRAP_16(wrap,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15)     wrap(a1),wrap(a2),wrap(a3),wrap(a4),wrap(a5),wrap(a6),wrap(a7),wrap(a8),wrap(a9),wrap(a10),wrap(a11),wrap(a12),wrap(a13),wrap(a14),wrap(a15)

#define VA_WRAP_(...)       OVERLOAD(VA_WRAP_, __VA_ARGS__)
#define VA_WRAP(name, ...)  VA_WRAP_(name, __VA_ARGS__)

#define VA_AGGREGATE_2(f,a1)                                                    (a1)
#define VA_AGGREGATE_3(f,a1,a2)                                                 f(a1,a2)
#define VA_AGGREGATE_4(f,a1,a2,a3)                                              f(f(a1,a2),a3)
#define VA_AGGREGATE_5(f,a1,a2,a3,a4)                                           f(f(f(a1,a2),a3),a4)
#define VA_AGGREGATE_6(f,a1,a2,a3,a4,a5)                                        f(f(f(f(a1,a2),a3),a4),a5)
#define VA_AGGREGATE_7(f,a1,a2,a3,a4,a5,a6)                                     f(f(f(f(f(a1,a2),a3),a4),a5),a6)
#define VA_AGGREGATE_8(f,a1,a2,a3,a4,a5,a6,a7)                                  f(f(f(f(f(f(a1,a2),a3),a4),a5),a6),a7)
#define VA_AGGREGATE_9(f,a1,a2,a3,a4,a5,a6,a7,a8)                               f(f(f(f(f(f(f(a1,a2),a3),a4),a5),a6),a7),a8)
#define VA_AGGREGATE_10(f,a1,a2,a3,a4,a5,a6,a7,a8,a9)                           f(f(f(f(f(f(f(f(a1,a2),a3),a4),a5),a6),a7),a8),a9)
#define VA_AGGREGATE_11(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10)                       f(f(f(f(f(f(f(f(f(a1,a2),a3),a4),a5),a6),a7),a8),a9),a10)
#define VA_AGGREGATE_12(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11)                   f(f(f(f(f(f(f(f(f(f(a1,a2),a3),a4),a5),a6),a7),a8),a9),a10),a11)
#define VA_AGGREGATE_13(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12)               f(f(f(f(f(f(f(f(f(f(f(a1,a2),a3),a4),a5),a6),a7),a8),a9),a10),a11),a12)
#define VA_AGGREGATE_14(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13)           f(f(f(f(f(f(f(f(f(f(f(f(a1,a2),a3),a4),a5),a6),a7),a8),a9),a10),a11),a12),a13)
#define VA_AGGREGATE_15(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14)       f(f(f(f(f(f(f(f(f(f(f(f(f(a1,a2),a3),a4),a5),a6),a7),a8),a9),a10),a11),a12),a13),a14)
#define VA_AGGREGATE_16(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15)   f(f(f(f(f(f(f(f(f(f(f(f(f(f(a1,a2),a3),a4),a5),a6),a7),a8),a9),a10),a11),a12),a13),a14),a15)

#define VA_AGGREGATE_(...)       OVERLOAD(VA_AGGREGATE_, __VA_ARGS__)
#define VA_AGGREGATE(name, ...)  VA_AGGREGATE_(name, __VA_ARGS__)

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

#define slist_stack_push(head, n)      SLSTACK_PUSH(*head, next, n)
#define slist_stack_pop(head)          SLSTACK_POP(*head, next)

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
// Floating point
//
//
////////////////////////////////////////////////////////////////

static inline f32 u32_as_f32(u32 u) {
    union { u32 u; f32 f; } v = { u };
    return v.f;
}

static inline u32 f32_as_u32(f32 f) {
    union { f32 f; u32 u; } v = { f };
    return v.u;
}

static inline f64 u64_as_f64(u64 u) {
    union { u64 u; f64 f; } v = { u };
    return v.f;
}

static inline u64 f64_as_u64(f64 f) {
    union { f64 f; u64 u; } v = { f };
    return v.u;
}

#define F32_FRACTION_MASK   ((u32)(0x007fffff))
#define F32_EXPONENT_MASK   ((u32)(0x7f800000))
#define F32_SIGN_MASK       ((u32)(0x80000000))

#define F64_FRACTION_MASK   ((u64)(0x000fffff'ffffffff))
#define F64_EXPONENT_MASK   ((u64)(0x7ff00000'00000000))
#define F64_SIGN_MASK       ((u64)(0x80000000'00000000))

#define F32_NAN             (u32_as_f32(0x7fbfffff))
#define F32_INF             (u32_as_f32(0x7f800000))
#define F32_MIN             (u32_as_f32(0xff7fffff)) // NOTE(rune): F32_MIN is different from FLT_MIN. F32_MIN == -FLT_MAX.
#define F32_MAX             (u32_as_f32(0x7f7fffff))

#define F64_NAN             (u64_as_f64(0x7ff80000'00000000))
#define F64_INF             (u64_as_f64(0x7ff00000'00000000))
#define F64_MIN             (u64_as_f64(0xffefffff'ffffffff)) // NOTE(rune): F64_MIN is different from DBL_MIN. F64_MIN == -DBL_MAX.
#define F64_MAX             (u64_as_f64(0x7fefffff'ffffffff))

static inline u32 f32_get_fraction_bits(f32 f) { return (f32_as_u32(f) & F32_FRACTION_MASK); }
static inline u32 f32_get_exponent_bits(f32 f) { return (f32_as_u32(f) & F32_EXPONENT_MASK); }
static inline u32 f32_get_sign_bit(f32 f) { return (f32_as_u32(f) & F32_SIGN_MASK); }
static inline bool f32_is_nan(f32 f) { return ((f32_as_u32(f) & F32_EXPONENT_MASK) == F32_EXPONENT_MASK) && ((f32_as_u32(f) & F32_FRACTION_MASK) != 0); }

static inline u64 f64_get_fraction_bits(f64 f) { return (f64_as_u64(f) & F64_FRACTION_MASK); }
static inline u64 f64_get_exponent_bits(f64 f) { return (f64_as_u64(f) & F64_EXPONENT_MASK); }
static inline u64 f64_get_sign_bit(f64 f) { return (f64_as_u64(f) & F64_SIGN_MASK); }
static inline bool f64_is_nan(f64 f) { return ((f64_as_u64(f) & F64_EXPONENT_MASK) == F64_EXPONENT_MASK) && ((f64_as_u64(f) & F32_FRACTION_MASK) != 0); }

static inline f32 f32_mod(f32 a, f32 b) { return fmodf(a, b); }
static inline f32 f32_sqrt(f32 a) { return sqrtf(a); }

////////////////////////////////////////////////////////////////
//
//
// Floating point vectors
//
//
////////////////////////////////////////////////////////////////

#define VEC2_ZERO (vec2(0.0f, 0.0f))
#define VEC3_ZERO (vec3(0.0f, 0.0f, 0.0f))
#define VEC4_ZERO (vec4(0.0f, 0.0f, 0.0f, 0.0f))

#define VEC2_ONE (vec2(1.0f, 1.0f))
#define VEC3_ONE (vec3(1.0f, 1.0f, 1.0f))
#define VEC4_ONE (vec4(1.0f, 1.0f, 1.0f, 1.0f))

#define VEC2_MIN (vec2(F32_MIN, F32_MIN))
#define VEC3_MIN (vec3(F32_MIN, F32_MIN, F32_MIN))
#define VEC4_MIN (vec4(F32_MIN, F32_MIN, F32_MIN, F32_MIN))

#define VEC2_MAX (vec2(F32_MAX, F32_MAX))
#define VEC3_MAX (vec3(F32_MAX, F32_MAX, F32_MAX))
#define VEC4_MAX (vec4(F32_MAX, F32_MAX, F32_MAX, F32_MAX))

#define VEC2_NAN (vec2(F32_NAN, F32_NAN))
#define VEC3_NAN (vec3(F32_NAN, F32_NAN, F32_NAN))
#define VEC4_NAN (vec4(F32_NAN, F32_NAN, F32_NAN, F32_NAN))

typedef union vec2 vec2;
union vec2 {
    struct { f32 v[2]; };
    struct { f32 x, y; };
};

typedef union vec3 vec3;
union vec3 {
    struct { f32 v[3]; };
    struct { f32 x, y, z; };
    struct { f32 r, g, b; };
};

typedef union vec4 vec4;
union vec4 {
    struct { f32 v[4]; };
    struct { f32 x, y, z, w; };
    struct { f32 r, g, b, a; };
};

#define vec2(x, y) (make_vec2((x), (y)))
#define vec3(x, y, z) (make_vec3((x), (y), (z)))
#define vec4(x, y, z, w) (make_vec4((x), (y), (z), (w)))

static inline vec2 make_vec2(f32 x, f32 y) { return lit(vec2) { x, y }; }
static inline vec3 make_vec3(f32 x, f32 y, f32 z) { return lit(vec3) { x, y, z }; }
static inline vec4 make_vec4(f32 x, f32 y, f32 z, f32 w) { return lit(vec4) { x, y, z, w }; }

static inline bool vec2_eq(vec2 a, vec2 b) { return (a.x == b.x) && (a.y == b.y); }
static inline bool vec3_eq(vec3 a, vec3 b) { return (a.x == b.x) && (a.y == b.y) && (a.z == b.z); }
static inline bool vec4_eq(vec4 a, vec4 b) { return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w); }
static inline vec2 vec2_add(vec2 a, vec2 b) { return vec2(a.x + b.x, a.y + b.y); }
static inline vec3 vec3_add(vec3 a, vec3 b) { return vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
static inline vec4 vec4_add(vec4 a, vec4 b) { return vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
static inline vec2 vec2_sub(vec2 a, vec2 b) { return vec2(a.x - b.x, a.y - b.y); }
static inline vec3 vec3_sub(vec3 a, vec3 b) { return vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
static inline vec4 vec4_sub(vec4 a, vec4 b) { return vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
static inline vec2 vec2_min(vec2 a, vec2 b) { return vec2(min(a.x, b.x), min(a.y, b.y)); }
static inline vec3 vec3_min(vec3 a, vec3 b) { return vec3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)); }
static inline vec4 vec4_min(vec4 a, vec4 b) { return vec4(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w)); }
static inline vec2 vec2_max(vec2 a, vec2 b) { return vec2(max(a.x, b.x), max(a.y, b.y)); }
static inline vec3 vec3_max(vec3 a, vec3 b) { return vec3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)); }
static inline vec4 vec4_max(vec4 a, vec4 b) { return vec4(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w)); }
static inline vec2 vec2_clamp(vec2 a, vec2 l, vec2 h) { return vec2(clamp(a.x, l.x, h.x), clamp(a.y, l.y, h.y)); }
static inline vec3 vec3_clamp(vec3 a, vec3 l, vec3 h) { return vec3(clamp(a.x, l.x, h.x), clamp(a.y, l.y, h.y), clamp(a.z, l.z, h.z)); }
static inline vec4 vec4_clamp(vec4 a, vec4 l, vec4 h) { return vec4(clamp(a.x, l.x, h.x), clamp(a.y, l.y, h.y), clamp(a.z, l.z, h.z), clamp(a.w, l.w, h.w)); }
static inline vec2 vec2_scale(vec2 a, f32 b) { return vec2(a.x * b, a.y * b); }
static inline vec3 vec3_scale(vec3 a, f32 b) { return vec3(a.x * b, a.y * b, a.z * b); }
static inline vec4 vec4_scale(vec4 a, f32 b) { return vec4(a.x * b, a.y * b, a.z * b, a.w * b); }

static inline vec2 vec2_add_assign(vec2 *a, vec2 b) { return *a = vec2_add(*a, b); }
static inline vec3 vec3_add_assign(vec3 *a, vec3 b) { return *a = vec3_add(*a, b); }
static inline vec4 vec4_add_assign(vec4 *a, vec4 b) { return *a = vec4_add(*a, b); }
static inline vec2 vec2_sub_assign(vec2 *a, vec2 b) { return *a = vec2_sub(*a, b); }
static inline vec3 vec3_sub_assign(vec3 *a, vec3 b) { return *a = vec3_sub(*a, b); }
static inline vec4 vec4_sub_assign(vec4 *a, vec4 b) { return *a = vec4_sub(*a, b); }
static inline vec2 vec2_min_assign(vec2 *a, vec2 b) { return *a = vec2_min(*a, b); }
static inline vec3 vec3_min_assign(vec3 *a, vec3 b) { return *a = vec3_min(*a, b); }
static inline vec4 vec4_min_assign(vec4 *a, vec4 b) { return *a = vec4_min(*a, b); }
static inline vec2 vec2_max_assign(vec2 *a, vec2 b) { return *a = vec2_max(*a, b); }
static inline vec3 vec3_max_assign(vec3 *a, vec3 b) { return *a = vec3_max(*a, b); }
static inline vec4 vec4_max_assign(vec4 *a, vec4 b) { return *a = vec4_max(*a, b); }
static inline vec2 vec2_clamp_assign(vec2 *a, vec2 l, vec2 h) { return *a = vec2_clamp(*a, l, h); }
static inline vec3 vec3_clamp_assign(vec3 *a, vec3 l, vec3 h) { return *a = vec3_clamp(*a, l, h); }
static inline vec4 vec4_clamp_assign(vec4 *a, vec4 l, vec4 h) { return *a = vec4_clamp(*a, l, h); }
static inline vec2 vec2_scale_assign(vec2 *a, f32 b) { return *a = vec2_scale(*a, b); }
static inline vec3 vec3_scale_assign(vec3 *a, f32 b) { return *a = vec3_scale(*a, b); }
static inline vec4 vec4_scale_assign(vec4 *a, f32 b) { return *a = vec4_scale(*a, b); }

static inline f32 vec2_len_sq(vec2 a) { return (a.x * a.x) + (a.y * a.y); }
static inline f32 vec3_len_sq(vec3 a) { return (a.x * a.x) + (a.y * a.y) + (a.z * a.z); }
static inline f32 vec4_len_sq(vec4 a) { return (a.x * a.x) + (a.y * a.y) + (a.z * a.z) + (a.w * a.w); }
static inline f32 vec2_len(vec2 a) { return f32_sqrt(vec2_len_sq(a)); }
static inline f32 vec3_len(vec3 a) { return f32_sqrt(vec3_len_sq(a)); }
static inline f32 vec4_len(vec4 a) { return f32_sqrt(vec4_len_sq(a)); }

#define vec2_sum(...) VA_AGGREGATE(vec2_add, __VA_ARGS__)
#define vec3_sum(...) VA_AGGREGATE(vec3_add, __VA_ARGS__)
#define vec4_sum(...) VA_AGGREGATE(vec4_add, __VA_ARGS__)

////////////////////////////////////////////////////////////////
//
//
// Signed integer vectors
//
//
////////////////////////////////////////////////////////////////

#define IVEC2_ZERO (ivec2(0, 0))
#define IVEC3_ZERO (ivec3(0, 0, 0))
#define IVEC4_ZERO (ivec4(0, 0, 0, 0))

#define IVEC2_ONE (ivec2(1, 1))
#define IVEC3_ONE (ivec3(1, 1, 1))
#define IVEC4_ONE (ivec4(1, 1, 1, 1))

#define IVEC2_MIN (ivec2(I32_MIN, I32_MIN))
#define IVEC3_MIN (ivec3(I32_MIN, I32_MIN, I32_MIN))
#define IVEC4_MIN (ivec4(I32_MIN, I32_MIN, I32_MIN, I32_MIN))

#define IVEC2_MAX (ivec2(I32_MAX, I32_MAX))
#define IVEC3_MAX (ivec3(I32_MAX, I32_MAX, I32_MAX))
#define IVEC4_MAX (ivec4(I32_MAX, I32_MAX, I32_MAX, I32_MAX))

typedef union ivec2 ivec2;
union ivec2 {
    struct { i32 v[2]; };
    struct { i32 x, y; };
};

typedef union ivec3 ivec3;
union ivec3 {
    struct { i32 v[3]; };
    struct { i32 x, y, z; };
    struct { i32 r, g, b; };
};

typedef union ivec4 ivec4;
union ivec4 {
    struct { i32 v[4]; };
    struct { i32 x, y, z, w; };
    struct { i32 r, g, b, a; };
};

#define ivec2(x, y) (make_ivec2((x), (y)))
#define ivec3(x, y, z) (make_ivec3((x), (y), (z)))
#define ivec4(x, y, z, w) (make_ivec4((x), (y), (z), (w)))

static inline ivec2 make_ivec2(i32 x, i32 y) { return lit(ivec2) { x, y }; }
static inline ivec3 make_ivec3(i32 x, i32 y, i32 z) { return lit(ivec3) { x, y, z }; }
static inline ivec4 make_ivec4(i32 x, i32 y, i32 z, i32 w) { return lit(ivec4) { x, y, z, w }; }

static inline bool ivec2_eq(ivec2 a, ivec2 b) { return (a.x == b.x) && (a.y == b.y); }
static inline bool ivec3_eq(ivec3 a, ivec3 b) { return (a.x == b.x) && (a.y == b.y) && (a.z == b.z); }
static inline bool ivec4_eq(ivec4 a, ivec4 b) { return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w); }
static inline ivec2 ivec2_add(ivec2 a, ivec2 b) { return ivec2(a.x + b.x, a.y + b.y); }
static inline ivec3 ivec3_add(ivec3 a, ivec3 b) { return ivec3(a.x + b.x, a.y + b.y, a.z + b.z); }
static inline ivec4 ivec4_add(ivec4 a, ivec4 b) { return ivec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
static inline ivec2 ivec2_sub(ivec2 a, ivec2 b) { return ivec2(a.x - b.x, a.y - b.y); }
static inline ivec3 ivec3_sub(ivec3 a, ivec3 b) { return ivec3(a.x - b.x, a.y - b.y, a.z - b.z); }
static inline ivec4 ivec4_sub(ivec4 a, ivec4 b) { return ivec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
static inline ivec2 ivec2_min(ivec2 a, ivec2 b) { return ivec2(min(a.x, b.x), min(a.y, b.y)); }
static inline ivec3 ivec3_min(ivec3 a, ivec3 b) { return ivec3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)); }
static inline ivec4 ivec4_min(ivec4 a, ivec4 b) { return ivec4(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w)); }
static inline ivec2 ivec2_max(ivec2 a, ivec2 b) { return ivec2(max(a.x, b.x), max(a.y, b.y)); }
static inline ivec3 ivec3_max(ivec3 a, ivec3 b) { return ivec3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)); }
static inline ivec4 ivec4_max(ivec4 a, ivec4 b) { return ivec4(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w)); }
static inline ivec2 ivec2_clamp(ivec2 a, ivec2 l, ivec2 h) { return ivec2(clamp(a.x, l.x, h.x), clamp(a.y, l.y, h.y)); }
static inline ivec3 ivec3_clamp(ivec3 a, ivec3 l, ivec3 h) { return ivec3(clamp(a.x, l.x, h.x), clamp(a.y, l.y, h.y), clamp(a.z, l.z, h.z)); }
static inline ivec4 ivec4_clamp(ivec4 a, ivec4 l, ivec4 h) { return ivec4(clamp(a.x, l.x, h.x), clamp(a.y, l.y, h.y), clamp(a.z, l.z, h.z), clamp(a.w, l.w, h.w)); }
static inline ivec2 ivec2_scale(ivec2 a, i32 b) { return ivec2(a.x * b, a.y * b); }
static inline ivec3 ivec3_scale(ivec3 a, i32 b) { return ivec3(a.x * b, a.y * b, a.z * b); }
static inline ivec4 ivec4_scale(ivec4 a, i32 b) { return ivec4(a.x * b, a.y * b, a.z * b, a.w * b); }

static inline ivec2 ivec2_add_assign(ivec2 *a, ivec2 b) { return *a = ivec2_add(*a, b); }
static inline ivec3 ivec3_add_assign(ivec3 *a, ivec3 b) { return *a = ivec3_add(*a, b); }
static inline ivec4 ivec4_add_assign(ivec4 *a, ivec4 b) { return *a = ivec4_add(*a, b); }
static inline ivec2 ivec2_sub_assign(ivec2 *a, ivec2 b) { return *a = ivec2_sub(*a, b); }
static inline ivec3 ivec3_sub_assign(ivec3 *a, ivec3 b) { return *a = ivec3_sub(*a, b); }
static inline ivec4 ivec4_sub_assign(ivec4 *a, ivec4 b) { return *a = ivec4_sub(*a, b); }
static inline ivec2 ivec2_min_assign(ivec2 *a, ivec2 b) { return *a = ivec2_min(*a, b); }
static inline ivec3 ivec3_min_assign(ivec3 *a, ivec3 b) { return *a = ivec3_min(*a, b); }
static inline ivec4 ivec4_min_assign(ivec4 *a, ivec4 b) { return *a = ivec4_min(*a, b); }
static inline ivec2 ivec2_max_assign(ivec2 *a, ivec2 b) { return *a = ivec2_max(*a, b); }
static inline ivec3 ivec3_max_assign(ivec3 *a, ivec3 b) { return *a = ivec3_max(*a, b); }
static inline ivec4 ivec4_max_assign(ivec4 *a, ivec4 b) { return *a = ivec4_max(*a, b); }
static inline ivec2 ivec2_clamp_assign(ivec2 *a, ivec2 l, ivec2 h) { return *a = ivec2_clamp(*a, l, h); }
static inline ivec3 ivec3_clamp_assign(ivec3 *a, ivec3 l, ivec3 h) { return *a = ivec3_clamp(*a, l, h); }
static inline ivec4 ivec4_clamp_assign(ivec4 *a, ivec4 l, ivec4 h) { return *a = ivec4_clamp(*a, l, h); }
static inline ivec2 ivec2_scale_assign(ivec2 *a, i32 b) { return *a = ivec2_scale(*a, b); }
static inline ivec3 ivec3_scale_assign(ivec3 *a, i32 b) { return *a = ivec3_scale(*a, b); }
static inline ivec4 ivec4_scale_assign(ivec4 *a, i32 b) { return *a = ivec4_scale(*a, b); }

#define ivec2_sum(...) VA_AGGREGATE(ivec2_add, __VA_ARGS__)
#define ivec3_sum(...) VA_AGGREGATE(ivec3_add, __VA_ARGS__)
#define ivec4_sum(...) VA_AGGREGATE(ivec4_add, __VA_ARGS__)

////////////////////////////////////////////////////////////////
//
//
// Unsigned integer vectors
//
//
////////////////////////////////////////////////////////////////

#define UVEC2_ZERO (uvec2(0, 0))
#define UVEC3_ZERO (uvec3(0, 0, 0))
#define UVEC4_ZERO (uvec4(0, 0, 0, 0))

#define UVEC2_ONE (uvec2(1, 1))
#define UVEC3_ONE (uvec3(1, 1, 1))
#define UVEC4_ONE (uvec4(1, 1, 1, 1))

#define UVEC2_MIN (uvec2(U32_MIN, U32_MIN))
#define UVEC3_MIN (uvec3(U32_MIN, U32_MIN, U32_MIN))
#define UVEC4_MIN (uvec4(U32_MIN, U32_MIN, U32_MIN, U32_MIN))

#define UVEC2_MAX (uvec2(U32_MAX, U32_MAX))
#define UVEC3_MAX (uvec3(U32_MAX, U32_MAX, U32_MAX))
#define UVEC4_MAX (uvec4(U32_MAX, U32_MAX, U32_MAX, U32_MAX))

typedef union uvec2 uvec2;
union uvec2 {
    struct { u32 v[2]; };
    struct { u32 x, y; };
};

typedef union uvec3 uvec3;
union uvec3 {
    struct { u32 v[3]; };
    struct { u32 x, y, z; };
    struct { u32 r, g, b; };
};

typedef union uvec4 uvec4;
union uvec4 {
    struct { u32 v[4]; };
    struct { u32 x, y, z, w; };
    struct { u32 r, g, b, a; };
};

#define uvec2(x, y) (make_uvec2((x), (y)))
#define uvec3(x, y, z) (make_uvec3((x), (y), (z)))
#define uvec4(x, y, z, w) (make_uvec4((x), (y), (z), (w)))

static inline uvec2 make_uvec2(u32 x, u32 y) { return lit(uvec2) { x, y }; }
static inline uvec3 make_uvec3(u32 x, u32 y, u32 z) { return lit(uvec3) { x, y, z }; }
static inline uvec4 make_uvec4(u32 x, u32 y, u32 z, u32 w) { return lit(uvec4) { x, y, z, w }; }

static inline bool uvec2_eq(uvec2 a, uvec2 b) { return (a.x == b.x) && (a.y == b.y); }
static inline bool uvec3_eq(uvec3 a, uvec3 b) { return (a.x == b.x) && (a.y == b.y) && (a.z == b.z); }
static inline bool uvec4_eq(uvec4 a, uvec4 b) { return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w); }
static inline uvec2 uvec2_add(uvec2 a, uvec2 b) { return uvec2(a.x + b.x, a.y + b.y); }
static inline uvec3 uvec3_add(uvec3 a, uvec3 b) { return uvec3(a.x + b.x, a.y + b.y, a.z + b.z); }
static inline uvec4 uvec4_add(uvec4 a, uvec4 b) { return uvec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
static inline uvec2 uvec2_sub(uvec2 a, uvec2 b) { return uvec2(a.x - b.x, a.y - b.y); }
static inline uvec3 uvec3_sub(uvec3 a, uvec3 b) { return uvec3(a.x - b.x, a.y - b.y, a.z - b.z); }
static inline uvec4 uvec4_sub(uvec4 a, uvec4 b) { return uvec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
static inline uvec2 uvec2_min(uvec2 a, uvec2 b) { return uvec2(min(a.x, b.x), min(a.y, b.y)); }
static inline uvec3 uvec3_min(uvec3 a, uvec3 b) { return uvec3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)); }
static inline uvec4 uvec4_min(uvec4 a, uvec4 b) { return uvec4(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w)); }
static inline uvec2 uvec2_max(uvec2 a, uvec2 b) { return uvec2(max(a.x, b.x), max(a.y, b.y)); }
static inline uvec3 uvec3_max(uvec3 a, uvec3 b) { return uvec3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)); }
static inline uvec4 uvec4_max(uvec4 a, uvec4 b) { return uvec4(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w)); }
static inline uvec2 uvec2_clamp(uvec2 a, uvec2 l, uvec2 h) { return uvec2(clamp(a.x, l.x, h.x), clamp(a.y, l.y, h.y)); }
static inline uvec3 uvec3_clamp(uvec3 a, uvec3 l, uvec3 h) { return uvec3(clamp(a.x, l.x, h.x), clamp(a.y, l.y, h.y), clamp(a.z, l.z, h.z)); }
static inline uvec4 uvec4_clamp(uvec4 a, uvec4 l, uvec4 h) { return uvec4(clamp(a.x, l.x, h.x), clamp(a.y, l.y, h.y), clamp(a.z, l.z, h.z), clamp(a.w, l.w, h.w)); }
static inline uvec2 uvec2_scale(uvec2 a, u32 b) { return uvec2(a.x * b, a.y * b); }
static inline uvec3 uvec3_scale(uvec3 a, u32 b) { return uvec3(a.x * b, a.y * b, a.z * b); }
static inline uvec4 uvec4_scale(uvec4 a, u32 b) { return uvec4(a.x * b, a.y * b, a.z * b, a.w * b); }

static inline uvec2 uvec2_add_assign(uvec2 *a, uvec2 b) { return *a = uvec2_add(*a, b); }
static inline uvec3 uvec3_add_assign(uvec3 *a, uvec3 b) { return *a = uvec3_add(*a, b); }
static inline uvec4 uvec4_add_assign(uvec4 *a, uvec4 b) { return *a = uvec4_add(*a, b); }
static inline uvec2 uvec2_sub_assign(uvec2 *a, uvec2 b) { return *a = uvec2_sub(*a, b); }
static inline uvec3 uvec3_sub_assign(uvec3 *a, uvec3 b) { return *a = uvec3_sub(*a, b); }
static inline uvec4 uvec4_sub_assign(uvec4 *a, uvec4 b) { return *a = uvec4_sub(*a, b); }
static inline uvec2 uvec2_min_assign(uvec2 *a, uvec2 b) { return *a = uvec2_min(*a, b); }
static inline uvec3 uvec3_min_assign(uvec3 *a, uvec3 b) { return *a = uvec3_min(*a, b); }
static inline uvec4 uvec4_min_assign(uvec4 *a, uvec4 b) { return *a = uvec4_min(*a, b); }
static inline uvec2 uvec2_max_assign(uvec2 *a, uvec2 b) { return *a = uvec2_max(*a, b); }
static inline uvec3 uvec3_max_assign(uvec3 *a, uvec3 b) { return *a = uvec3_max(*a, b); }
static inline uvec4 uvec4_max_assign(uvec4 *a, uvec4 b) { return *a = uvec4_max(*a, b); }
static inline uvec2 uvec2_clamp_assign(uvec2 *a, uvec2 l, uvec2 h) { return *a = uvec2_clamp(*a, l, h); }
static inline uvec3 uvec3_clamp_assign(uvec3 *a, uvec3 l, uvec3 h) { return *a = uvec3_clamp(*a, l, h); }
static inline uvec4 uvec4_clamp_assign(uvec4 *a, uvec4 l, uvec4 h) { return *a = uvec4_clamp(*a, l, h); }
static inline uvec2 uvec2_scale_assign(uvec2 *a, u32 b) { return *a = uvec2_scale(*a, b); }
static inline uvec3 uvec3_scale_assign(uvec3 *a, u32 b) { return *a = uvec3_scale(*a, b); }
static inline uvec4 uvec4_scale_assign(uvec4 *a, u32 b) { return *a = uvec4_scale(*a, b); }

#define uvec2_sum(...) VA_AGGREGATE(uvec2_add, __VA_ARGS__)
#define uvec3_sum(...) VA_AGGREGATE(uvec3_add, __VA_ARGS__)
#define uvec4_sum(...) VA_AGGREGATE(uvec4_add, __VA_ARGS__)

////////////////////////////////////////////////////////////////
//
//
// Vector conversions
//
//
////////////////////////////////////////////////////////////////

static inline vec2 vec2_from_ivec2(ivec2 v) { return vec2((f32)v.x, (f32)v.y); }
static inline ivec2 ivec2_from_vec2(vec2 v) { return ivec2((i32)v.x, (i32)v.y); }

////////////////////////////////////////////////////////////////
//
//
// Ranges
//
//
////////////////////////////////////////////////////////////////

typedef struct i8_range { i8  min, max; } i8_range;
typedef struct i16_range { i16 min, max; } i16_range;
typedef struct i32_range { i32 min, max; } i32_range;
typedef struct i64_range { i64 min, max; } i64_range;
typedef struct u8_range { u8  min, max; } u8_range;
typedef struct u16_range { u16 min, max; } u16_range;
typedef struct u32_range { u32 min, max; } u32_range;
typedef struct u64_range { u64 min, max; } u64_range;
typedef struct f32_range { f32 min, max; } f32_range;
typedef struct f64_range { f64 min, max; } f64_range;

#define i8_range(a, b)  make_range_i8(a, b)
#define i16_range(a, b) make_range_i16(a, b)
#define i32_range(a, b) make_range_i32(a, b)
#define i64_range(a, b) make_range_i64(a, b)

#define u8_range(a, b)  make_range_u8(a, b)
#define u16_range(a, b) make_range_u16(a, b)
#define u32_range(a, b) make_range_u32(a, b)
#define u64_range(a, b) make_range_u64(a, b)

#define f32_range(a, b) make_range_f32(a, b)
#define f64_range(a, b) make_range_f64(a, b)

static inline i8_range make_range_i8(i8 a, i8 b) { return a < b ? lit(i8_range) { a, b } : lit(i8_range) { b, a }; }
static inline i16_range make_range_i16(i16 a, i16 b) { return a < b ? lit(i16_range) { a, b } : lit(i16_range) { b, a }; }
static inline i32_range make_range_i32(i32 a, i32 b) { return a < b ? lit(i32_range) { a, b } : lit(i32_range) { b, a }; }
static inline i64_range make_range_i64(i64 a, i64 b) { return a < b ? lit(i64_range) { a, b } : lit(i64_range) { b, a }; }

static inline u8_range make_range_u8(u8 a, u8 b) { return a < b ? lit(u8_range) { a, b } : lit(u8_range) { b, a }; }
static inline u16_range make_range_u16(u16 a, u16 b) { return a < b ? lit(u16_range) { a, b } : lit(u16_range) { b, a }; }
static inline u32_range make_range_u32(u32 a, u32 b) { return a < b ? lit(u32_range) { a, b } : lit(u32_range) { b, a }; }
static inline u64_range make_range_u64(u64 a, u64 b) { return a < b ? lit(u64_range) { a, b } : lit(u64_range) { b, a }; }

static inline f32_range make_range_f32(f32 a, f32 b) { return a < b ? lit(f32_range) { a, b } : lit(f32_range) { b, a }; }
static inline f64_range make_range_f64(f64 a, f64 b) { return a < b ? lit(f64_range) { a, b } : lit(f64_range) { b, a }; }

#define range_contains(r, a) ((a) >= (r).min && a <  (r).max)
#define range_len(r) ((r).max - (r).min)
#define ranges_overlap(a, b)   ((a.min < b.max) && (a.max > b.min))

////////////////////////////////////////////////////////////////
//
//
// Floating point rectangle
//
//
////////////////////////////////////////////////////////////////

typedef union rect rect;
union rect {
    struct { vec2 p[2]; };
    struct { vec2 p0, p1; };
    struct { f32 x0, y0, x1, y1; };
};

static inline rect make_rect(vec2 p0, vec2 p1) { return lit(rect) { p0, p1 }; }
static inline rect make_rect_xy(f32 x, f32 y) { return make_rect(vec2(0, 0), vec2(x, y)); }
static inline rect make_rect_xyxy(f32 x0, f32 y0, f32 x1, f32 y1) { return make_rect(vec2(x0, y0), vec2(x1, y1)); }
static inline rect make_rect_xxyy(f32 x0, f32 x1, f32 y0, f32 y1) { return make_rect(vec2(x0, y0), vec2(x1, y1)); }
static inline rect make_rect_dim(vec2 p0, vec2 dim) { return lit(rect) { p0, vec2_add(p0, dim) }; }

static inline bool rect_eq(rect a, rect b) { return vec2_eq(a.p0, b.p0) && vec2_eq(a.p1, b.p1); }
static inline vec2 rect_dim(rect r) { return vec2_sub(r.p1, r.p0); }
static inline f32 rect_dim_x(rect r) { return r.x1 - r.x0; }
static inline f32 rect_dim_y(rect r) { return r.y1 - r.y0; }
static inline f32 rect_dim_a(rect r, axis2 axis) { return axis == AXIS2_X ? rect_dim_x(r) : rect_dim_y(r); }
static inline rect rect_offset(rect r, vec2 off) { return make_rect(vec2_add(r.p0, off), vec2_add(r.p1, off)); }
static inline rect rect_intersect(rect a, rect b) { return make_rect(vec2_max(a.p0, b.p0), vec2_min(a.p1, b.p1)); }
static inline rect rect_union(rect a, rect b) { return make_rect(vec2_min(a.p0, b.p0), vec2_max(a.p1, b.p1)); }
static inline rect rect_offset_assign(rect *a, vec2 b) { return *a = rect_offset(*a, b); }
static inline rect rect_intersect_assign(rect *a, rect b) { return *a = rect_intersect(*a, b); }
static inline rect rect_union_assign(rect *a, rect b) { return *a = rect_union(*a, b); }
static inline f32_range rect_range_x(rect a) { return f32_range(a.x0, a.x1); }
static inline f32_range rect_range_y(rect a) { return f32_range(a.y0, a.y1); }
static inline bool rect_contains(rect r, vec2 p) { return (p.x >= r.x0 && p.x < r.x1 && p.y >= r.y0 && p.y < r.y1); }
static inline bool rect_contains_incl(rect r, vec2 p) { return (p.x >= r.x0 && p.x <= r.x1 && p.y >= r.y0 && p.y <= r.y1); }
static inline bool rect_overlaps(rect a, rect b) { return (a.y0 <= b.y1 && a.x0 < b.x1 && b.y0 <= a.y1 && b.x0 < a.x1); }
static inline bool rect_overlaps_incl(rect a, rect b) { return (a.y0 <= b.y1 && a.x0 <= b.x1 && b.y0 <= a.y1 && b.x0 <= a.x1); }
static inline rect rect_clamp_dim(rect a, vec2 min, vec2 max) { return make_rect_dim(a.p0, vec2_clamp(rect_dim(a), min, max)); }

////////////////////////////////////////////////////////////////
//
//
// Signed integer rectangle
//
//
////////////////////////////////////////////////////////////////

#define irect(p0,p1) make_irect(p0,p1)

typedef union irect irect;
union irect {
    struct { ivec2 p[2]; };
    struct { ivec2 p0, p1; };
    struct { i32 x0, y0, x1, y1; };
};

static inline irect make_irect(ivec2 p0, ivec2 p1) { return lit(irect) { p0, p1 }; }
static inline irect make_irect_xyxy(i32 x0, i32 y0, i32 x1, i32 y1) { return make_irect(ivec2(x0, y0), ivec2(x1, y1)); }
static inline irect make_irect_xxyy(i32 x0, i32 x1, i32 y0, i32 y1) { return make_irect(ivec2(x0, y0), ivec2(x1, y1)); }
static inline irect make_irect_dim(ivec2 p0, ivec2 dim) { return lit(irect) { p0, ivec2_add(p0, dim) }; }

static inline bool irect_eq(irect a, irect b) { return ivec2_eq(a.p0, b.p0) && ivec2_eq(a.p1, b.p1); }
static inline ivec2 irect_dim(irect r) { return ivec2_sub(r.p1, r.p0); }
static inline i32 irect_dim_x(irect r) { return r.x1 - r.x0; }
static inline i32 irect_dim_y(irect r) { return r.y1 - r.y0; }
static inline i32 irect_dim_a(irect r, axis2 axis) { return axis == AXIS2_X ? irect_dim_x(r) : irect_dim_y(r); }
static inline irect irect_offset(irect r, ivec2 off) { return make_irect(ivec2_add(r.p0, off), ivec2_add(r.p1, off)); }
static inline irect irect_intersect(irect a, irect b) { return make_irect(ivec2_max(a.p0, b.p0), ivec2_min(a.p1, b.p1)); }
static inline irect irect_union(irect a, irect b) { return make_irect(ivec2_min(a.p0, b.p0), ivec2_max(a.p1, b.p1)); }
static inline irect irect_offset_assign(irect *a, ivec2 b) { return *a = irect_offset(*a, b); }
static inline irect irect_intersect_assign(irect *a, irect b) { return *a = irect_intersect(*a, b); }
static inline irect irect_union_assign(irect *a, irect b) { return *a = irect_union(*a, b); }
static inline i32_range irect_range_x(irect a) { return i32_range(a.x0, a.x1); }
static inline i32_range irect_range_y(irect a) { return i32_range(a.y0, a.y1); }
static inline bool irect_contains(irect r, ivec2 p) { return (p.x >= r.x0 && p.x < r.x1 && p.y >= r.y0 && p.y < r.y1); }
static inline bool irect_contains_incl(irect r, ivec2 p) { return (p.x >= r.x0 && p.x <= r.x1 && p.y >= r.y0 && p.y <= r.y1); }
static inline bool irect_overlaps(irect a, irect b) { return (a.y0 <= b.y1 && a.x0 < b.x1 && b.y0 <= a.y1 && b.x0 < a.x1); }
static inline bool irect_overlaps_incl(irect a, irect b) { return (a.y0 <= b.y1 && a.x0 <= b.x1 && b.y0 <= a.y1 && b.x0 <= a.x1); }
static inline irect irect_clamp_dim(irect a, ivec2 min, ivec2 max) { return make_irect_dim(a.p0, ivec2_clamp(irect_dim(a), min, max)); }

////////////////////////////////////////////////////////////////
//
//
// Unsigned integer rectangle
//
//
////////////////////////////////////////////////////////////////

#define urect(p0,p1) make_urect(p0,p1)

typedef union urect urect;
union urect {
    struct { uvec2 p[2]; };
    struct { uvec2 p0, p1; };
    struct { u32 x0, y0, x1, y1; };
};

static inline urect make_urect(uvec2 p0, uvec2 p1) { return lit(urect) { p0, p1 }; }
static inline urect make_urect_xyxy(u32 x0, u32 y0, u32 x1, u32 y1) { return make_urect(uvec2(x0, y0), uvec2(x1, y1)); }
static inline urect make_urect_xxyy(u32 x0, u32 x1, u32 y0, u32 y1) { return make_urect(uvec2(x0, y0), uvec2(x1, y1)); }
static inline urect make_urect_dim(uvec2 p0, uvec2 dim) { return lit(urect) { p0, uvec2_add(p0, dim) }; }

static inline bool urect_eq(urect a, urect b) { return uvec2_eq(a.p0, b.p0) && uvec2_eq(a.p1, b.p1); }
static inline uvec2 urect_dim(urect r) { return uvec2_sub(r.p1, r.p0); }
static inline u32 urect_dim_x(urect r) { return r.x1 - r.x0; }
static inline u32 urect_dim_y(urect r) { return r.y1 - r.y0; }
static inline u32 urect_dim_a(urect r, axis2 axis) { return axis == AXIS2_X ? urect_dim_x(r) : urect_dim_y(r); }
static inline urect urect_offset(urect r, uvec2 off) { return make_urect(uvec2_add(r.p0, off), uvec2_add(r.p1, off)); }
static inline urect urect_intersect(urect a, urect b) { return make_urect(uvec2_max(a.p0, b.p0), uvec2_min(a.p1, b.p1)); }
static inline urect urect_union(urect a, urect b) { return make_urect(uvec2_min(a.p0, b.p0), uvec2_max(a.p1, b.p1)); }
static inline urect urect_offset_assign(urect *a, uvec2 b) { return *a = urect_offset(*a, b); }
static inline urect urect_intersect_assign(urect *a, urect b) { return *a = urect_intersect(*a, b); }
static inline urect urect_union_assign(urect *a, urect b) { return *a = urect_union(*a, b); }
static inline u32_range urect_range_x(urect a) { return u32_range(a.x0, a.x1); }
static inline u32_range urect_range_y(urect a) { return u32_range(a.y0, a.y1); }
static inline bool urect_contains(urect r, uvec2 p) { return (p.x >= r.x0 && p.x < r.x1 && p.y >= r.y0 && p.y < r.y1); }
static inline bool urect_contains_incl(urect r, uvec2 p) { return (p.x >= r.x0 && p.x <= r.x1 && p.y >= r.y0 && p.y <= r.y1); }
static inline bool urect_overlaps(urect a, urect b) { return (a.y0 <= b.y1 && a.x0 < b.x1 && b.y0 <= a.y1 && b.x0 < a.x1); }
static inline bool urect_overlaps_incl(urect a, urect b) { return (a.y0 <= b.y1 && a.x0 <= b.x1 && b.y0 <= a.y1 && b.x0 <= a.x1); }
static inline urect urect_clamp_dim(urect a, uvec2 min, uvec2 max) { return make_urect_dim(a.p0, uvec2_clamp(urect_dim(a), min, max)); }


////////////////////////////////////////////////////////////////
//
//
// Rectangle conversions
//
//
////////////////////////////////////////////////////////////////

static inline rect rect_from_irect(irect v) { return make_rect(vec2_from_ivec2(v.p0), vec2_from_ivec2(v.p1)); }
static inline irect irect_from_rect(rect v) { return make_irect(ivec2_from_vec2(v.p0), ivec2_from_vec2(v.p1)); }

////////////////////////////////////////////////////////////////
//
//
// Rounding
//
//
////////////////////////////////////////////////////////////////

static inline f32 f32_trunc(f32 f) { return (f32)(i32)(f); }
static inline f64 f64_trunc(f64 f) { return (f64)(i64)(f); }
static inline f32 f32_round(f32 f) { return (f32)(i32)(f + 0.5f); }
static inline f64 f64_round(f64 f) { return (f64)(i64)(f + 0.5); }
static inline f32 f32_abs(f32 f) { return f < 0 ? -f : f; }
static inline f64 f64_abs(f64 f) { return f < 0 ? -f : f; }
static inline i8  i8_abs(i8 i) { return i < 0 ? -i : i; }
static inline i16 i16_abs(i16 i) { return i < 0 ? -i : i; }
static inline i32 i32_abs(i32 i) { return i < 0 ? -i : i; }
static inline i64 i64_abs(i64 i) { return i < 0 ? -i : i; }

static inline f32 f32_trunc_assign(f32 *f) { return *f = f32_trunc(*f); }
static inline f64 f64_trunc_assign(f64 *f) { return *f = f64_trunc(*f); }
static inline f32 f32_round_assign(f32 *f) { return *f = f32_round(*f); }
static inline f64 f64_round_assign(f64 *f) { return *f = f64_round(*f); }
static inline f32 f32_abs_assign(f32 *f) { return *f = f32_abs(*f); }
static inline f64 f64_abs_assign(f64 *f) { return *f = f64_abs(*f); }
static inline i8  i8_abs_assign(i8 *i) { return *i = i8_abs(*i); }
static inline i16 i16_abs_assign(i16 *i) { return *i = i16_abs(*i); }
static inline i32 i32_abs_assign(i32 *i) { return *i = i32_abs(*i); }
static inline i64 i64_abs_assign(i64 *i) { return *i = i64_abs(*i); }

static inline vec2 vec2_trunc(vec2 v) { return vec2(f32_trunc(v.x), f32_trunc(v.y)); }
static inline vec3 vec3_trunc(vec3 v) { return vec3(f32_trunc(v.x), f32_trunc(v.y), f32_trunc(v.z)); }
static inline vec4 vec4_trunc(vec4 v) { return vec4(f32_trunc(v.x), f32_trunc(v.y), f32_trunc(v.z), f32_trunc(v.w)); }
static inline rect rect_trunc(rect v) { return make_rect(vec2_trunc(v.p0), vec2_trunc(v.p1)); }
static inline vec2 vec2_round(vec2 v) { return vec2(f32_round(v.x), f32_round(v.y)); }
static inline vec3 vec3_round(vec3 v) { return vec3(f32_round(v.x), f32_round(v.y), f32_round(v.z)); }
static inline vec4 vec4_round(vec4 v) { return vec4(f32_round(v.x), f32_round(v.y), f32_round(v.z), f32_round(v.w)); }
static inline rect rect_round(rect v) { return make_rect(vec2_round(v.p0), vec2_round(v.p1)); }
static inline vec2 vec2_abs(vec2 v) { return vec2(f32_abs(v.x), f32_abs(v.y)); }
static inline vec3 vec3_abs(vec3 v) { return vec3(f32_abs(v.x), f32_abs(v.y), f32_abs(v.z)); }
static inline vec4 vec4_abs(vec4 v) { return vec4(f32_abs(v.x), f32_abs(v.y), f32_abs(v.z), f32_abs(v.w)); }
static inline rect rect_abs(rect v) { return make_rect(vec2_abs(v.p0), vec2_abs(v.p1)); }

static inline vec2 vec2_trunc_assign(vec2 *v) { return *v = vec2_trunc(*v); }
static inline vec3 vec3_trunc_assign(vec3 *v) { return *v = vec3_trunc(*v); }
static inline vec4 vec4_trunc_assign(vec4 *v) { return *v = vec4_trunc(*v); }
static inline rect rect_trunc_assign(rect *v) { return *v = rect_trunc(*v); }
static inline vec2 vec2_round_assign(vec2 *v) { return *v = vec2_round(*v); }
static inline vec3 vec3_round_assign(vec3 *v) { return *v = vec3_round(*v); }
static inline vec4 vec4_round_assign(vec4 *v) { return *v = vec4_round(*v); }
static inline rect rect_round_assign(rect *v) { return *v = rect_round(*v); }
static inline vec2 vec2_abs_assign(vec2 *i) { return *i = vec2_abs(*i); }
static inline vec3 vec3_abs_assign(vec3 *i) { return *i = vec3_abs(*i); }
static inline vec4 vec4_abs_assign(vec4 *i) { return *i = vec4_abs(*i); }
static inline rect rect_abs_assign(rect *i) { return *i = rect_abs(*i); }

////////////////////////////////////////////////////////////////
//
//
// Modolu
//
//
////////////////////////////////////////////////////////////////

static inline i8 i8_mod(i8 a, i8 n) { return (a % n + n) % n; }
static inline i16 i16_mod(i16 a, i16 n) { return (a % n + n) % n; }
static inline i32 i32_mod(i32 a, i32 n) { return (a % n + n) % n; }
static inline i64 i64_mod(i64 a, i64 n) { return (a % n + n) % n; }

////////////////////////////////////////////////////////////////
//
//
// Lerp/smoothing/easing
//
//
////////////////////////////////////////////////////////////////

static inline f32 f32_lerp(f32 a, f32 b, f32 t) { return a + clamp01(t) * (b - a); }
static inline vec2 vec2_lerp(vec2 a, vec2 b, f32 t) { return vec2(f32_lerp(a.x, b.x, t), f32_lerp(a.y, b.y, t)); }
static inline vec3 vec3_lerp(vec3 a, vec3 b, f32 t) { return vec3(f32_lerp(a.x, b.x, t), f32_lerp(a.y, b.y, t), f32_lerp(a.z, b.z, t)); }
static inline vec4 vec4_lerp(vec4 a, vec4 b, f32 t) { return vec4(f32_lerp(a.x, b.x, t), f32_lerp(a.y, b.y, t), f32_lerp(a.z, b.z, t), f32_lerp(a.w, b.w, t)); }

static inline f32  f32_lerp_assign(f32 *a, f32 b, f32 t) { return *a = f32_lerp(*a, b, t); }
static inline vec2 vec2_lerp_assign(vec2 *a, vec2 b, f32 t) { return *a = vec2_lerp(*a, b, t); }
static inline vec3 vec3_lerp_assign(vec3 *a, vec3 b, f32 t) { return *a = vec3_lerp(*a, b, t); }
static inline vec4 vec4_lerp_assign(vec4 *a, vec4 b, f32 t) { return *a = vec4_lerp(*a, b, t); }

static inline f32 f32_ease_out(f32 t) { return 1 - (1 - clamp01(t)) * (1 - clamp01(t)); }
static inline f64 f64_ease_out(f64 t) { return 1 - (1 - clamp01(t)) * (1 - clamp01(t)); }
static inline f32 f32_ease_in(f32 t) { return clamp01(t) * clamp01(t); }
static inline f64 f64_ease_in(f64 t) { return clamp01(t) * clamp01(t); }

static inline f32 f32_smoothstep(f32 t) { return clamp01(t) * clamp01(t) * (3.0f - 2.0f * clamp01(t)); }
static inline f64 f64_smoothstep(f64 t) { return clamp01(t) * clamp01(t) * (3.0  - 2.0 * clamp01(t)); }

static inline f32 f32_smooth_damp(f32 current, f32 target, f32 *velocity, f32 duration, f32 max_speed, f32 dt) {
    // NOTE(rune): Copy pased from Unity's Mathf.SmoothDamp,
    // https://github.com/Unity-Technologies/UnityCsReference/blob/master/Runtime/Export/Math/Mathf.cs,
    // Based on Game Programming Gems 4 Chapter 1.10

    if (dt == 0) return current;

    duration = max(0.0001f, duration);
    f32 omega = 2.0f / duration;

    f32 x = omega * dt;
    f32 exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
    f32 change = current - target;
    f32 orig_target = target;

    // Clamp maximum speed
    f32 max_change = max_speed * duration;
    change = clamp(change, -max_change, max_change);
    target = current - change;

    f32 temp = (*velocity + omega * change) * dt;
    *velocity = (*velocity - omega * temp) * exp;
    f32 output = target + (change + temp) * exp;

    // Prevent overshooting
    if (orig_target - current > 0.0f == output > orig_target) {
        output = orig_target;
        *velocity = (output - orig_target) / dt;
    }
    return output;

}

static inline f32 f32_smooth_damp_assign(f32 *current, f32 target, f32 *velocity, f32 duration, f32 max_speed, f32 dt) {
    return *current = f32_smooth_damp(*current, target, velocity, duration, max_speed, dt);
}

////////////////////////////////////////////////////////////////
//
//
// Packing
//
//
////////////////////////////////////////////////////////////////

static inline u32 pack_u8x4(u8 u0, u8 u1, u8 u2, u8 u3) {
    return ((cast_u32(u0) <<  0) |
            (cast_u32(u1) <<  8) |
            (cast_u32(u2) << 16) |
            (cast_u32(u3) << 24));
}

static inline void unpack_u8x4(u32 u, u8 *u0, u8 *u1, u8 *u2, u8 *u3) {
    *u0 = cast_u8(u >>  0);
    *u1 = cast_u8(u >>  8);
    *u2 = cast_u8(u >> 16);
    *u3 = cast_u8(u >> 24);
}

static inline u32 pack_u16x2(u16 u0, u16 u1) {
    return ((cast_u32(u0) <<   0) |
            (cast_u32(u1) <<  16));
}

static inline void unpack_u16x2(u32 u, u16 *u0, u16 *u1) {
    *u0 = cast_u16(u >>  0);
    *u1 = cast_u16(u >> 16);
}

static inline u64 pack_u32x2(u32 a, u32 b) {
    return ((cast_u64(a) <<  0) |
            (cast_u64(b) << 32));
}

static inline void unpack_u32x2(u64 u, u32 *u0, u32 *u1) {
    *u0 = cast_u32(u >>  0);
    *u1 = cast_u32(u >> 32);
}

static inline u32 pack_rgba(vec4 v) {
    u8 r = cast_u8(v.r * 255.0f);
    u8 g = cast_u8(v.g * 255.0f);
    u8 b = cast_u8(v.b * 255.0f);
    u8 a = cast_u8(v.a * 255.0f);

    u32 u = pack_u8x4(r, g, b, a);
    return u;
}

static inline vec4 unpack_rgba(u32 u) {
    u8 r, g, b, a;
    unpack_u8x4(u, &r, &g, &b, &a);
    vec4 ret = { 0 };
    ret.r = cast_f32(r) / 255.0f;
    ret.g = cast_f32(g) / 255.0f;
    ret.b = cast_f32(b) / 255.0f;
    ret.a = cast_f32(a) / 255.0f;
    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Bits
//
//
////////////////////////////////////////////////////////////////

#define align_to_pow2(a, b) (((a) + (b) - 1) & ~((b) - 1))

static u8 u8_align_to_pow2(u8 a, u8 power_of_two) { return align_to_pow2(a, power_of_two); }
static u16 u16_align_to_pow2(u16 a, u16 power_of_two) { return align_to_pow2(a, power_of_two); }
static u32 u32_align_to_pow2(u32 a, u32 power_of_two) { return align_to_pow2(a, power_of_two); }
static u64 u64_align_to_pow2(u64 a, u64 power_of_two) { return align_to_pow2(a, power_of_two); }

static inline i32 u32_first_bit_set(u32 u) {
    i32 ret = -1;
    for_range (u32, i, 0, 32) {
        if (u & (1 << i)) {
            ret = i;
            break;
        }
    }
    return ret;
}

static inline i32 u32_pop_first_bit_set(u32 *u) {
    i32 ret = u32_first_bit_set(*u);
    if (ret != -1) {
        *u &= ~(1 << ret);
    }
    return ret;
}

static inline u32 u32_round_up_to_pow2(u32 u) {
    if (u == 0) {
        u = 1;
    } else {
        u -= 1;
        u |= (u >> 1);
        u |= (u >> 2);
        u |= (u >> 4);
        u |= (u >> 8);
        u |= (u >> 16);
        u += 1;
    }
    return u;
}

static inline u64 u64_round_up_to_pow2(u64 u) {
    if (u == 0) {
        u = 1;
    } else {
        u -= 1;
        u |= (u >> 1);
        u |= (u >> 2);
        u |= (u >> 4);
        u |= (u >> 8);
        u |= (u >> 16);
        u |= (u >> 32);
        u += 1;
    }
    return u;
}

static inline u64 u64_safe_sub(u64 a, u64 b) {
    if (a > b) {
        return a - b;
    } else {
        return 0;
    }
}

static inline u64 u64_safe_add(u64 a, u64 b) {
    if (a + b >= a) {
        return a + b;
    } else {
        return U64_MAX;
    }
}

static inline u64 u64_round_down(u64 a, u64 multiple) {
    return a - a % multiple;
}

static inline u64 u64_round_up(u64 a, u64 multiple) {
    return a + multiple - 1 - (a + multiple - 1) % multiple;
}

static inline void *ptr_round_down(void *ptr, u64 multiple) {
    return (void *)u64_round_down((u64)ptr, multiple);
}

static inline void *ptr_round_up(void *ptr, u64 multiple) {
    return (void *)u64_round_up((u64)ptr, multiple);
}

////////////////////////////////////////////////////////////////
//
//
// Any
//
//
////////////////////////////////////////////////////////////////

typedef enum any_tag {
    ANY_TAG_I8,
    ANY_TAG_I16,
    ANY_TAG_I32,
    ANY_TAG_I64,
    ANY_TAG_U8,
    ANY_TAG_U16,
    ANY_TAG_U32,
    ANY_TAG_U64,
    ANY_TAG_F32,
    ANY_TAG_F64,
    ANY_TAG_BOOL,
    ANY_TAG_STR,
    ANY_TAG_VEC2,
    ANY_TAG_VEC3,
    ANY_TAG_VEC4,
    ANY_TAG_RECT,
    ANY_TAG_RANGE_I8,
    ANY_TAG_RANGE_I16,
    ANY_TAG_RANGE_I32,
    ANY_TAG_RANGE_I64,
    ANY_TAG_RANGE_U8,
    ANY_TAG_RANGE_U16,
    ANY_TAG_RANGE_U32,
    ANY_TAG_RANGE_U64,
    ANY_TAG_RANGE_F32,
    ANY_TAG_RANGE_F64,
    ANY_TAG_PTR,
} any_tag;

typedef struct any any;
struct any {
    any_tag tag;
    union {
        i8 _i8;
        i16 _i16;
        i32 _i32;
        i64 _i64;
        u8 _u8;
        u16 _u16;
        u32 _u32;
        u64 _u64;
        f32 _f32;
        f64 _f64;
        bool _bool;
        char _char;
        str _str;
        vec2 _vec2;
        vec3 _vec3;
        vec4 _vec4;
        rect _rect;
        i8_range _range_i8;
        i16_range _range_i16;
        i32_range _range_i32;
        i64_range _range_i64;
        u8_range _range_u8;
        u16_range _range_u16;
        u32_range _range_u32;
        u64_range _range_u64;
        f32_range _range_f32;
        f64_range _range_f64;
        void *_void_ptr;
    };
};

typedef_span(any);
typedef any_span args;

#ifndef __cplusplus
#define anyof(a) (_Generic((a),                          \
        i8                  : make_any_i8,               \
        i16                 : make_any_i16,              \
        i32                 : make_any_i32,              \
        i64                 : make_any_i64,              \
        u8                  : make_any_u8,               \
        u16                 : make_any_u16,              \
        u32                 : make_any_u32,              \
        u64                 : make_any_u64,              \
        f32                 : make_any_f32,              \
        f64                 : make_any_f64,              \
        bool                : make_any_bool,             \
        u8_span             : make_any_str,              \
        union vec2          : make_any_vec2,             \
        union vec3          : make_any_vec3,             \
        union vec4          : make_any_vec4,             \
        union rect          : make_any_rect,             \
        i8_range            : make_any_range_i8,         \
        i16_range           : make_any_range_i16,        \
        i32_range           : make_any_range_i32,        \
        i64_range           : make_any_range_i64,        \
        u8_range            : make_any_range_u8,         \
        u16_range           : make_any_range_u16,        \
        u32_range           : make_any_range_u32,        \
        u64_range           : make_any_range_u64,        \
        f32_range           : make_any_range_f32,        \
        f64_range           : make_any_range_f64,        \
        char *              : make_any_cstr,             \
        void *              : make_any_ptr,              \
        struct any          : make_any_self,             \
        default             : make_any_ptr               \
))((a))

#define argsof(...)    (make_span(any, ((any[]) { VA_WRAP(anyof, __VA_ARGS__) }), NUM_ARGUMENTS(__VA_ARGS__)))

static inline any make_any_i8(i8 a) { return lit(any) { .tag = ANY_TAG_I8, ._i8 = a }; }
static inline any make_any_i16(i16 a) { return lit(any) { .tag = ANY_TAG_I16, ._i16 = a }; }
static inline any make_any_i32(i32 a) { return lit(any) { .tag = ANY_TAG_I32, ._i32 = a }; }
static inline any make_any_i64(i64 a) { return lit(any) { .tag = ANY_TAG_I64, ._i64 = a }; }
static inline any make_any_u8(u8 a) { return lit(any) { .tag = ANY_TAG_U8, ._u8 = a }; }
static inline any make_any_u16(u16 a) { return lit(any) { .tag = ANY_TAG_U16, ._u16 = a }; }
static inline any make_any_u32(u32 a) { return lit(any) { .tag = ANY_TAG_U32, ._u32 = a }; }
static inline any make_any_u64(u64 a) { return lit(any) { .tag = ANY_TAG_U64, ._u64 = a }; }
static inline any make_any_f32(f32 a) { return lit(any) { .tag = ANY_TAG_F32, ._f32 = a }; }
static inline any make_any_f64(f64 a) { return lit(any) { .tag = ANY_TAG_F64, ._f64 = a }; }
static inline any make_any_bool(bool a) { return lit(any) { .tag = ANY_TAG_BOOL, ._bool = a }; }
static inline any make_any_str(str a) { return lit(any) { .tag = ANY_TAG_STR, ._str = a }; }
static inline any make_any_vec2(vec2 a) { return lit(any) { .tag = ANY_TAG_VEC2, ._vec2 = a }; }
static inline any make_any_vec3(vec3 a) { return lit(any) { .tag = ANY_TAG_VEC3, ._vec3 = a }; }
static inline any make_any_vec4(vec4 a) { return lit(any) { .tag = ANY_TAG_VEC4, ._vec4 = a }; }
static inline any make_any_rect(rect a) { return lit(any) { .tag = ANY_TAG_RECT, ._rect = a }; }
static inline any make_any_range_i8(i8_range a) { return lit(any) { .tag = ANY_TAG_RANGE_I8, ._range_i8 = a }; }
static inline any make_any_range_i16(i16_range a) { return lit(any) { .tag = ANY_TAG_RANGE_I16, ._range_i16 = a }; }
static inline any make_any_range_i32(i32_range a) { return lit(any) { .tag = ANY_TAG_RANGE_I32, ._range_i32 = a }; }
static inline any make_any_range_i64(i64_range a) { return lit(any) { .tag = ANY_TAG_RANGE_I64, ._range_i64 = a }; }
static inline any make_any_range_u8(u8_range a) { return lit(any) { .tag = ANY_TAG_RANGE_U8, ._range_u8 = a }; }
static inline any make_any_range_u16(u16_range a) { return lit(any) { .tag = ANY_TAG_RANGE_U16, ._range_u16 = a }; }
static inline any make_any_range_u32(u32_range a) { return lit(any) { .tag = ANY_TAG_RANGE_U32, ._range_u32 = a }; }
static inline any make_any_range_u64(u64_range a) { return lit(any) { .tag = ANY_TAG_RANGE_U64, ._range_u64 = a }; }
static inline any make_any_range_f32(f32_range a) { return lit(any) { .tag = ANY_TAG_RANGE_F32, ._range_f32 = a }; }
static inline any make_any_range_f64(f64_range a) { return lit(any) { .tag = ANY_TAG_RANGE_F64, ._range_f64 = a }; }
static inline any make_any_cstr(char *a) { return lit(any) { .tag = ANY_TAG_STR, ._str.v = (u8 *)a, ._str.len = strlen(a) }; }
static inline any make_any_ptr(void *a) { return lit(any) { .tag = ANY_TAG_PTR, ._void_ptr = a }; }
static inline any make_any_self(any a) { return a; }

#endif

////////////////////////////////////////////////////////////////
//
//
// Color
//
//
////////////////////////////////////////////////////////////////

// NOTE(rune): Copy-pasted from https://github.com/ocornut/imgui.
static void rgb_to_hsv(f32 r, f32 g, f32 b, f32 *out_h, f32 *out_s, f32 *out_v) {
    f32 K = 0.f;
    if (g < b) {
        swap(f32, &g, &b);
        K = -1.f;
    }
    if (r < g) {
        swap(f32, &r, &g);
        K = -2.f / 6.f - K;
    }

    f32 chroma = r - (g < b ? g : b);
    *out_h = f32_abs(K + (g - b) / (6.f * chroma + 1e-20f));
    *out_s = chroma / (r + 1e-20f);
    *out_v = r;
}

// NOTE(rune): Copy-pasted from https://github.com/ocornut/imgui.
static void hsv_to_rgb(f32 h, f32 s, f32 v, f32 *out_r, f32 *out_g, f32 *out_b) {
    if (s == 0.0f) {
        *out_r = *out_g = *out_b = v;
        return;
    }

    h = f32_mod(h, 1.0f) / (60.0f / 360.0f);
    i32 i = (i32)h;
    f32 f = h - (f32)i;
    f32 p = v * (1.0f - s);
    f32 q = v * (1.0f - s * f);
    f32 t = v * (1.0f - s * (1.0f - f));

    switch (i) {
        case 0: *out_r = v; *out_g = t; *out_b = p; break;
        case 1: *out_r = q; *out_g = v; *out_b = p; break;
        case 2: *out_r = p; *out_g = v; *out_b = t; break;
        case 3: *out_r = p; *out_g = q; *out_b = v; break;
        case 4: *out_r = t; *out_g = p; *out_b = v; break;
        case 5: default: *out_r = v; *out_g = p; *out_b = q; break;
    }
}

static inline u32 hsv_to_rgb32(f32 h, f32 s, f32 v) {
    f32 r, g, b;
    hsv_to_rgb(h, s, v, &r, &g, &b);
    u32 u32 = pack_rgba(vec4(r, g, b, 1));
    return u32;
}

static inline void rgb32_to_hsv(u32 rgba, f32 *h, f32 *s, f32 *v) {
    vec4 vec = unpack_rgba(rgba);
    rgb_to_hsv(vec.r, vec.g, vec.b, h, s, v);
}