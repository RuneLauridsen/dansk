////////////////////////////////////////////////////////////////
//
//
// Warnings
//
//
////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4201) // C4201: nonstandard extension used: nameless struct/union
#pragma warning ( disable : 4127) // C4127: conditional expression is constant
#pragma warning ( disable : 4702) // C4702: unreachable code
#pragma warning ( disable : 4505) // C4505: unreferenced function with internal linkage has been removed

#if 1 // Disable warning for unused locals/parameters?
#pragma warning (disable : 4189) // C4189: local variable is initialized but not referenced
#pragma warning (disable : 4101) // C4101: unreferenced local variable
#pragma warning (disable : 4100) // C4100: unreferenced formal parameter
#endif

////////////////////////////////////////////////////////////////
//
//
// Scalars
//
//
////////////////////////////////////////////////////////////////

typedef intptr_t    ix;
typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;
typedef uintptr_t   ux;
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
#define global static

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
#define clamp(a,l,h)    (min(max(a,l),h))
#define clamp01(a)      (clamp(a,0,1))
#define clamp_bot(a,l)  (max((a), (l)))
#define clamp_top(a,h)  (min((a), (h)))

#define min_assign(a,b)         (*(a) = min(*(a),b))
#define max_assign(a,b)         (*(a) = max(*(a),b))
#define clamp_assign(a,l,h)     (*(a) = clamp(*(a),l,h))
#define clamp01_assign(a)       (*(a) = clamp01(*(a)))
#define clamp_bot_assign(a,l)   (*(a) = clamp_bot(*(a),l))
#define clamp_top_assign(a,h)   (*(a) = clamp_top(*(a),h))

#ifdef __cplusplus
#define lit(T) T
#define extern_c extern "C"
#else
#define lit(T) (T)
#define extern_c extern
#endif

#define cast(T, a)  ((T)(a))
#define ix(a)       ((ix)(a))
#define i8(a)       ((i8)(a))
#define i16(a)      ((i16)(a))
#define i32(a)      ((i32)(a))
#define i64(a)      ((i64)(a))
#define ux(a)       ((ux)(a))
#define u8(a)       ((u8)(a))
#define u16(a)      ((u16)(a))
#define u32(a)      ((u32)(a))
#define u64(a)      ((u64)(a))
#define b8(a)       ((b8)(a))
#define b16(a)      ((b16)(a))
#define b32(a)      ((b32)(a))
#define b64(a)      ((b64)(a))
#define f32(a)      ((f32)(a))
#define f64(a)      ((f64)(a))

#define rgba(r,g,b,a)   (v4(f32(r)/255.0f, f32(g)/255.0f, f32(b)/255.0f, f32(a)/255.0f))
#define rgb(r,g,b)      (v4(f32(r)/255.0f, f32(g)/255.0f, f32(b)/255.0f, 1.0f))

#define rgba32(r,g,b,a) (pack_u8x4(u8(r), u8(g), u8(b), u8(a)))
#define rgb32(r,g,b)    (pack_u8x4(u8(r), u8(g), u8(b), u8(255)))

#define defer_scope__(begin, end, i) for(int _defer##i##_ = ((begin), 0); _defer##i##_ == 0; _defer##i##_ += 1, (end))
#define defer_scope_(begin, end, i)  defer_scope__(begin, end, i)
#define defer_scope(begin, end)      defer_scope_(begin, end, __LINE__)

#define sign(a)     ((a) < 0 ? -1 : 1)

#define template_(name, T) name##_##T
#define template(name, T)  template_(name, T)

#define for_range(T, it, start, end)  for(T it = start; it < end; it++)

#define for_val_count(T, it, items, count) for(ix __temp##__LINE__ = 0, it##_idx = 0; __temp##__LINE__ == 0; __temp##__LINE__++) for(T it = {0}; (it##_idx < (count)) ? (it = (items)[it##_idx], true) : (false); it##_idx++)
#define for_ptr_count(T, it, items, count) for(ix __temp##__LINE__ = 0, it##_idx = 0; __temp##__LINE__ == 0; __temp##__LINE__++) for(T *it = (items); it##_idx < (count); it##_idx++, it++)

#define for_val(T, it, items) for_val_count(T, it, (items).v, (items).count)
#define for_ptr(T, it, items) for_ptr_count(T, it, (items).v, (items).count)

#define for_val_array(T, it, array) for_val_count(T, it, (array), countof(array))
#define for_ptr_array(T, it, array) for_ptr_count(T, it, (array), countof(array))

#define for_val_list(T, it, list) for (T *__ptr_##it = (list), it; (__ptr_##it && (it = *__ptr_##it, 1)); __ptr_##it = __ptr_##it->next)
#define for_ptr_list(T, it, list) for (T *it = (list); it; it = it->next)

////////////////////////////////////////////////////////////////
//
//
// Assertions
//
//
////////////////////////////////////////////////////////////////

#define debug_break() __debugbreak()

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

#define assert_bounds(span, idx) (assert_bounds_((idx), (span).count))

static void assert_bounds_(ix idx, ix max) {
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

typedef struct { void *v; union { ix count, len; }; } span_void;

#define span(T) span_##T
#define span_struct(T)              \
union {                             \
    struct {                        \
        T *v;                       \
        union { ix count, len; };   \
    };                              \
    struct {                        \
        span_void _void;            \
    };                              \
}

#define typedef_span(T) typedef span_struct(T) span(T)

typedef_span(ix);
typedef_span(i8);
typedef_span(i16);
typedef_span(i32);
typedef_span(i64);
typedef_span(ux);
typedef_span(u8);
typedef_span(u16);
typedef_span(u32);
typedef_span(u64);
typedef_span(f32);
typedef_span(f64);

#define make_span(T, v, count)           (lit(span(T)) { (v), (count) })

typedef span_u8 span;
typedef span_u8 str;
typedef span_u16 wstr;

typedef_span(str);
typedef_span(wstr);

static str make_str(u8 *v, ix count) { return make_span(u8, v, count); }
static wstr make_wstr(u16 *v, ix count) { return make_span(u16, v, count); }

#define list(T)                        struct { T *first, *last; }
#define slist_node(T)                  struct { T v, *next; }
#define dlist_node(T)                  struct { T v, *next, *prev; }

#define span_literal(T, ...)          (make_span(T, ((T[]) { __VA_ARGS__ }), NUM_ARGUMENTS(__VA_ARGS__)))

#define static_str(text)                 { .v = (u8 *)(text), .len = lengthof(text)}
#define static_make_span(T, v, count)    { .v = (v), .count = (count) }
#define static_span(T, ...)              { .v = (T[]) { __VA_ARGS__ }, .count = NUM_ARGUMENTS(__VA_ARGS__) }
#define static_strs(...)                 { .v = (str[]) { VA_WRAP(static_str, __VA_ARGS__)}, .count = NUM_ARGUMENTS(__VA_ARGS__) }

#define str(text)                      make_str((u8 *)(text), lengthof(text))
#define spanof(T, array)               make_span(T, array, countof(array))

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
#define VA_WRAP_2(wrap,a1)                                                      wrap((a1))
#define VA_WRAP_3(wrap,a1,a2)                                                   wrap((a1)),wrap((a2))
#define VA_WRAP_4(wrap,a1,a2,a3)                                                wrap((a1)),wrap((a2)),wrap((a3))
#define VA_WRAP_5(wrap,a1,a2,a3,a4)                                             wrap((a1)),wrap((a2)),wrap((a3)),wrap((a4))
#define VA_WRAP_6(wrap,a1,a2,a3,a4,a5)                                          wrap((a1)),wrap((a2)),wrap((a3)),wrap((a4)),wrap((a5))
#define VA_WRAP_7(wrap,a1,a2,a3,a4,a5,a6)                                       wrap((a1)),wrap((a2)),wrap((a3)),wrap((a4)),wrap((a5)),wrap((a6))
#define VA_WRAP_8(wrap,a1,a2,a3,a4,a5,a6,a7)                                    wrap((a1)),wrap((a2)),wrap((a3)),wrap((a4)),wrap((a5)),wrap((a6)),wrap((a7))
#define VA_WRAP_9(wrap,a1,a2,a3,a4,a5,a6,a7,a8)                                 wrap((a1)),wrap((a2)),wrap((a3)),wrap((a4)),wrap((a5)),wrap((a6)),wrap((a7)),wrap((a8))
#define VA_WRAP_10(wrap,a1,a2,a3,a4,a5,a6,a7,a8,a9)                             wrap((a1)),wrap((a2)),wrap((a3)),wrap((a4)),wrap((a5)),wrap((a6)),wrap((a7)),wrap((a8)),wrap((a9))
#define VA_WRAP_11(wrap,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10)                         wrap((a1)),wrap((a2)),wrap((a3)),wrap((a4)),wrap((a5)),wrap((a6)),wrap((a7)),wrap((a8)),wrap((a9)),wrap((a1)0)
#define VA_WRAP_12(wrap,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11)                     wrap((a1)),wrap((a2)),wrap((a3)),wrap((a4)),wrap((a5)),wrap((a6)),wrap((a7)),wrap((a8)),wrap((a9)),wrap((a1)0),wrap((a1)1)
#define VA_WRAP_13(wrap,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12)                 wrap((a1)),wrap((a2)),wrap((a3)),wrap((a4)),wrap((a5)),wrap((a6)),wrap((a7)),wrap((a8)),wrap((a9)),wrap((a1)0),wrap((a1)1),wrap((a1)2)
#define VA_WRAP_14(wrap,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13)             wrap((a1)),wrap((a2)),wrap((a3)),wrap((a4)),wrap((a5)),wrap((a6)),wrap((a7)),wrap((a8)),wrap((a9)),wrap((a1)0),wrap((a1)1),wrap((a1)2),wrap((a1)3)
#define VA_WRAP_15(wrap,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14)         wrap((a1)),wrap((a2)),wrap((a3)),wrap((a4)),wrap((a5)),wrap((a6)),wrap((a7)),wrap((a8)),wrap((a9)),wrap((a1)0),wrap((a1)1),wrap((a1)2),wrap((a1)3),wrap((a1)4)
#define VA_WRAP_16(wrap,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15)     wrap((a1)),wrap((a2)),wrap((a3)),wrap((a4)),wrap((a5)),wrap((a6)),wrap((a7)),wrap((a8)),wrap((a9)),wrap((a1)0),wrap((a1)1),wrap((a1)2),wrap((a1)3),wrap((a1)4),wrap((a1)5)

#define VA_WRAP_(...)       OVERLOAD(VA_WRAP_, __VA_ARGS__)
#define VA_WRAP(name, ...)  VA_WRAP_(name, __VA_ARGS__)

////////////////////////////////////////////////////////////////
//
//
// Floating point
//
//
////////////////////////////////////////////////////////////////

static f32 u32_as_f32(u32 u) {
    union { u32 u; f32 f; } v = { u };
    return v.f;
}

static u32 f32_as_u32(f32 f) {
    union { f32 f; u32 u; } v = { f };
    return v.u;
}

#define F32_NAN             (u32_as_f32(0x7fbfffff))
#define F32_INFINITY        (u32_as_f32(0x7f800000))
#define F32_MIN             (u32_as_f32(0xff7fffff)) // NOTE(rune): F32_MIN is differnt from FLT_MIN. F32_MIN == -F32_MAX.
#define F32_MAX             (u32_as_f32(0x7f7fffff))

#define F32_FRACTION_MASK   (0x007fffff)
#define F32_EXPONENT_MASK   (0x7f800000)
#define F32_SIGN_MASK       (0x80000000)

static u32 f32_get_fraction_bits(f32 f) { return (f32_as_u32(f) & F32_FRACTION_MASK); }
static u32 f32_get_exponent_bits(f32 f) { return (f32_as_u32(f) & F32_EXPONENT_MASK); }
static u32 f32_get_sign_bit(f32 f) { return (f32_as_u32(f) & F32_SIGN_MASK); }
static bool f32_is_nan(f32 f) { return ((f32_as_u32(f) & F32_EXPONENT_MASK) == F32_EXPONENT_MASK) && ((f32_as_u32(f) & F32_FRACTION_MASK) != 0); }

////////////////////////////////////////////////////////////////
//
//
// Floating point vectors
//
//
////////////////////////////////////////////////////////////////

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

#define V2_MIN  (v2(F32_MIN, F32_MIN))
#define V3_MIN  (v3(F32_MIN, F32_MIN, F32_MIN))
#define V4_MIN  (v2(F32_MIN, F32_MIN, F32_MIN, F32_MIN))

#define V2_MAX  (v2(F32_MAX, F32_MAX))
#define V3_MAX  (v3(F32_MAX, F32_MAX, F32_MAX))
#define V4_MAX  (v2(F32_MAX, F32_MAX, F32_MAX, F32_MAX))

#define V2_NAN  (v2(F32_NAN, F32_NAN))
#define V3_NAN  (v3(F32_NAN, F32_NAN, F32_NAN))
#define V4_NAN  (v2(F32_NAN, F32_NAN, F32_NAN, F32_NAN))

static vec2 v2(f32 x, f32 y) { return lit(vec2) { x, y }; }
static vec3 v3(f32 x, f32 y, f32 z) { return lit(vec3) { x, y, z }; }
static vec4 v4(f32 x, f32 y, f32 z, f32 w) { return lit(vec4) { x, y, z, w }; }

static bool v2_eq(vec2 a, vec2 b) { return (a.x == b.x) && (a.y == b.y); }
static vec2 v2_neg(vec2 a) { return v2(-a.x, -a.y); }
static vec2 v2_add(vec2 a, vec2 b) { return v2(a.x + b.x, a.y + b.y); }
static vec2 v2_sub(vec2 a, vec2 b) { return v2(a.x - b.x, a.y - b.y); }
static vec2 v2_min(vec2 a, vec2 b) { return v2(min(a.x, b.x), min(a.y, b.y)); }
static vec2 v2_max(vec2 a, vec2 b) { return v2(max(a.x, b.x), max(a.y, b.y)); }
static vec2 v2_clamp(vec2 a, vec2 l, vec2 h) { return v2(clamp(a.x, l.x, h.x), clamp(a.y, l.y, h.y)); }

static void v2_add_assign(vec2 *a, vec2 b) { *a = v2_add(*a, b); }
static void v2_sub_assign(vec2 *a, vec2 b) { *a = v2_sub(*a, b); }
static void v2_min_assign(vec2 *a, vec2 b) { *a = v2_min(*a, b); }
static void v2_max_assign(vec2 *a, vec2 b) { *a = v2_max(*a, b); }
static void v2_clamp_assign(vec2 *a, vec2 l, vec2 h) { *a = v2_clamp(*a, l, h); }

////////////////////////////////////////////////////////////////
//
//
// Integer vectors
//
//
////////////////////////////////////////////////////////////////

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

static ivec2 v2i(i32 x, i32 y) { return lit(ivec2) { x, y }; }
static ivec3 v3i(i32 x, i32 y, i32 z) { return lit(ivec3) { x, y, z }; }
static ivec4 v4i(i32 x, i32 y, i32 z, i32 w) { return lit(ivec4) { x, y, z, w }; }

static bool v2i_eq(ivec2 a, ivec2 b) { return (a.x == b.x) && (a.y == b.y); }
static ivec2 v2i_neg(ivec2 a) { return v2i(-a.x, -a.y); }
static ivec2 v2i_add(ivec2 a, ivec2 b) { return v2i(a.x + b.x, a.y + b.y); }
static ivec2 v2i_sub(ivec2 a, ivec2 b) { return v2i(a.x - b.x, a.y - b.y); }
static ivec2 v2i_min(ivec2 a, ivec2 b) { return v2i(min(a.x, b.x), min(a.y, b.y)); }
static ivec2 v2i_max(ivec2 a, ivec2 b) { return v2i(max(a.x, b.x), max(a.y, b.y)); }
static ivec2 v2i_clamp(ivec2 a, ivec2 l, ivec2 h) { return v2i(clamp(a.x, l.x, h.x), clamp(a.y, l.y, h.y)); }

static ivec2 v2i_add_assign(ivec2 *a, ivec2 b) { *a = v2i_add(*a, b); }
static ivec2 v2i_sub_assign(ivec2 *a, ivec2 b) { *a = v2i_sub(*a, b); }
static ivec2 v2i_min_assign(ivec2 *a, ivec2 b) { *a = v2i_min(*a, b); }
static ivec2 v2i_max_assign(ivec2 *a, ivec2 b) { *a = v2i_max(*a, b); }
static ivec2 v2i_clamp_assign(ivec2 *a, ivec2 l, ivec2 h) { *a = v2i_clamp(*a, l, h); }

////////////////////////////////////////////////////////////////
//
//
// Vector conversions
//
//
////////////////////////////////////////////////////////////////

static vec2 v2_from_v2i(ivec2 v) { return v2((f32)v.x, (f32)v.y); }
static ivec2 v2i_from_v2(vec2 v) { return v2i((i32)v.x, (i32)v.y); }

////////////////////////////////////////////////////////////////
//
//
// Ranges
//
//
////////////////////////////////////////////////////////////////

typedef struct { ix  min, max; } range_ix;
typedef struct { i8  min, max; } range_i8;
typedef struct { i16 min, max; } range_i16;
typedef struct { i32 min, max; } range_i32;
typedef struct { i64 min, max; } range_i64;
typedef struct { ux  min, max; } range_ux;
typedef struct { u8  min, max; } range_u8;
typedef struct { u16 min, max; } range_u16;
typedef struct { u32 min, max; } range_u32;
typedef struct { u64 min, max; } range_u64;
typedef struct { f32 min, max; } range_f32;
typedef struct { f64 min, max; } range_f64;

#define range_ix(a, b)  make_range_ix(a, b)
#define range_i8(a, b)  make_range_i8(a, b)
#define range_i16(a, b) make_range_i16(a, b)
#define range_i32(a, b) make_range_i32(a, b)
#define range_i64(a, b) make_range_i64(a, b)

#define range_ux(a, b)  make_range_ux(a, b)
#define range_u8(a, b)  make_range_u8(a, b)
#define range_u16(a, b) make_range_u16(a, b)
#define range_u32(a, b) make_range_u32(a, b)
#define range_u64(a, b) make_range_u64(a, b)

#define range_f32(a, b) make_range_f32(a, b)
#define range_f64(a, b) make_range_f64(a, b)

static range_ix make_range_ix(ix a, ix b) { return a < b ? lit(range_ix) { a, b } : lit(range_ix) { b, a }; }
static range_i8 make_range_i8(i8 a, i8 b) { return a < b ? lit(range_i8) { a, b } : lit(range_i8) { b, a }; }
static range_i16 make_range_i16(i16 a, i16 b) { return a < b ? lit(range_i16) { a, b } : lit(range_i16) { b, a }; }
static range_i32 make_range_i32(i32 a, i32 b) { return a < b ? lit(range_i32) { a, b } : lit(range_i32) { b, a }; }
static range_i64 make_range_i64(i64 a, i64 b) { return a < b ? lit(range_i64) { a, b } : lit(range_i64) { b, a }; }

static range_ux make_range_ux(ux a, ux b) { return a < b ? lit(range_ux) { a, b } : lit(range_ux) { b, a }; }
static range_u8 make_range_u8(u8 a, u8 b) { return a < b ? lit(range_u8) { a, b } : lit(range_u8) { b, a }; }
static range_u16 make_range_u16(u16 a, u16 b) { return a < b ? lit(range_u16) { a, b } : lit(range_u16) { b, a }; }
static range_u32 make_range_u32(u32 a, u32 b) { return a < b ? lit(range_u32) { a, b } : lit(range_u32) { b, a }; }
static range_u64 make_range_u64(u64 a, u64 b) { return a < b ? lit(range_u64) { a, b } : lit(range_u64) { b, a }; }

static range_f32 make_range_f32(f32 a, f32 b) { return a < b ? lit(range_f32) { a, b } : lit(range_f32) { b, a }; }
static range_f64 make_range_f64(f64 a, f64 b) { return a < b ? lit(range_f64) { a, b } : lit(range_f64) { b, a }; }

#define in_range(r, a) ((a) >= (r).min && a <  (r).max)
#define range_len(r) ((r).max - (r).min)

////////////////////////////////////////////////////////////////
//
//
// Rectangles
//
//
////////////////////////////////////////////////////////////////

typedef union rect rect;
union rect {
    struct { vec2 p[2]; };
    struct { vec2 p0, p1; };
    struct { f32 x0, y0, x1, y1; };
};

#define rect(p0,p1) make_rect(p0,p1)

static rect make_rect(vec2 p0, vec2 p1) { return lit(rect) { p0, p1 }; }
static rect make_rect_dim(vec2 p0, vec2 dim) { return lit(rect) { p0, v2_add(p0, dim) }; }

static bool rect_eq(rect a, rect b) { return v2_eq(a.p0, b.p0) && v2_eq(a.p1, b.p1); }
static vec2 rect_dim(rect r) { return v2_sub(r.p1, r.p0); }
static f32 rect_dim_x(rect r) { return r.x1 - r.x0; }
static f32 rect_dim_y(rect r) { return r.y1 - r.y0; }
static rect rect_offset(rect r, vec2 off) { return rect(v2_add(r.p0, off), v2_add(r.p1, off)); }
static rect rect_intersect(rect a, rect b) { return rect(v2_max(a.p0, b.p0), v2_min(a.p1, b.p1)); }
static rect rect_union(rect a, rect b) { return rect(v2_min(a.p0, b.p0), v2_max(a.p1, b.p1)); }
static void rect_offset_assign(rect *a, vec2 b) { *a = rect_offset(*a, b); }
static void rect_intersect_assign(rect *a, rect b) { *a = rect_intersect(*a, b); }
static void rect_union_assign(rect *a, rect b) { *a = rect_union(*a, b); }
static range_f32 rect_range_x(rect a) { return range_f32(a.x0, a.x1); }
static range_f32 rect_range_y(rect a) { return range_f32(a.y0, a.y1); }

#define irect(p0,p1) make_irect(p0,p1)

typedef union irect irect;
union irect {
    struct { ivec2 p[2]; };
    struct { ivec2 p0, p1; };
    struct { i32 x0, y0, x1, y1; };
};

static irect make_irect(ivec2 p0, ivec2 p1) { return lit(irect) { p0, p1 }; }
static irect make_irect_dim(ivec2 p0, ivec2 dim) { return lit(irect) { p0, v2i_add(p0, dim) }; }

static bool irect_eq(irect a, irect b) { return v2i_eq(a.p0, b.p0) && v2i_eq(a.p1, b.p1); }
static i32 irect_width(irect r) { return r.p1.x - r.p0.x; }
static i32 irect_height(irect r) { return r.p1.y - r.p0.y; }
static ivec2 irect_dim(irect r) { return v2i_sub(r.p1, r.p0); }
static irect irect_offset(irect r, ivec2 off) { return irect(v2i_add(r.p0, off), v2i_add(r.p1, off)); }
static irect irect_intersect(irect a, irect b) { return irect(v2i_max(a.p0, b.p0), v2i_min(a.p1, b.p1)); }
static irect irect_union(irect a, irect b) { return irect(v2i_min(a.p0, b.p0), v2i_max(a.p1, b.p1)); }
static void irect_intersect_assign(irect *a, irect b) { *a = irect_intersect(*a, b); }
static void irect_union_assign(irect *a, irect b) { *a = irect_union(*a, b); }
static range_i32 irect_range_x(irect a) { return range_i32(a.x0, a.x1); }
static range_i32 irect_range_y(irect a) { return range_i32(a.y0, a.y1); }

////////////////////////////////////////////////////////////////
//
//
// Rounding
//
//
////////////////////////////////////////////////////////////////

static f32 f32_trunc(f32 f) { return (f32)(i32)(f); }
static f64 f64_trunc(f64 f) { return (f64)(i64)(f); }
static f32 f32_round(f32 f) { return (f32)(i32)(f + 0.5f); }
static f64 f64_round(f64 f) { return (f64)(i64)(f + 0.5); }
static f32 f32_abs(f32 f) { return f < 0 ? -f : f; }
static f64 f64_abs(f64 f) { return f < 0 ? -f : f; }
static ix ix_abs(ix i) { return i < 0 ? -i : i; }
static i8 i8_abs(i8 i) { return i < 0 ? -i : i; }
static i16 i16_abs(i16 i) { return i < 0 ? -i : i; }
static i32 i32_abs(i32 i) { return i < 0 ? -i : i; }
static i64 i64_abs(i64 i) { return i < 0 ? -i : i; }

static void f32_trunc_assign(f32 *f) { *f = f32_trunc(*f); }
static void f64_trunc_assign(f64 *f) { *f = f64_trunc(*f); }
static void f32_round_assign(f32 *f) { *f = f32_round(*f); }
static void f64_round_assign(f64 *f) { *f = f64_round(*f); }
static void f32_abs_assign(f32 *f) { *f = f32_abs(*f); }
static void f64_abs_assign(f64 *f) { *f = f64_abs(*f); }
static void ix_abs_assign(ix *i) { *i = ix_abs(*i); }
static void i8_abs_assign(i8 *i) { *i = i8_abs(*i); }
static void i16_abs_assign(i16 *i) { *i = i16_abs(*i); }
static void i32_abs_assign(i32 *i) { *i = i32_abs(*i); }
static void i64_abs_assign(i64 *i) { *i = i64_abs(*i); }

static vec2 v2_trunc(vec2 v) { return v2(f32_trunc(v.x), f32_trunc(v.y)); }
static vec3 v3_trunc(vec3 v) { return v3(f32_trunc(v.x), f32_trunc(v.y), f32_trunc(v.z)); }
static vec4 v4_trunc(vec4 v) { return v4(f32_trunc(v.x), f32_trunc(v.y), f32_trunc(v.z), f32_trunc(v.w)); }
static rect rect_trunc(rect v) { return rect(v2_trunc(v.p0), v2_trunc(v.p1)); }
static vec2 v2_round(vec2 v) { return v2(f32_round(v.x), f32_round(v.y)); }
static vec3 v3_round(vec3 v) { return v3(f32_round(v.x), f32_round(v.y), f32_round(v.z)); }
static vec4 v4_round(vec4 v) { return v4(f32_round(v.x), f32_round(v.y), f32_round(v.z), f32_round(v.w)); }
static rect rect_round(rect v) { return rect(v2_round(v.p0), v2_round(v.p1)); }
static vec2 v2_abs(vec2 v) { return v2(f32_abs(v.x), f32_abs(v.y)); }
static vec3 v3_abs(vec3 v) { return v3(f32_abs(v.x), f32_abs(v.y), f32_abs(v.z)); }
static vec4 v4_abs(vec4 v) { return v4(f32_abs(v.x), f32_abs(v.y), f32_abs(v.z), f32_abs(v.w)); }
static rect rect_abs(rect v) { return rect(v2_abs(v.p0), v2_abs(v.p1)); }

static void v2_trunc_assign(vec2 *v) { *v = v2_trunc(*v); }
static void v3_trunc_assign(vec3 *v) { *v = v3_trunc(*v); }
static void v4_trunc_assign(vec4 *v) { *v = v4_trunc(*v); }
static void rect_trunc_assign(rect *v) { *v = rect_trunc(*v); }
static void v2_round_assign(vec2 *v) { *v = v2_round(*v); }
static void v3_round_assign(vec3 *v) { *v = v3_round(*v); }
static void v4_round_assign(vec4 *v) { *v = v4_round(*v); }
static void v2_abs_assign(vec2 *i) { *i = v2_abs(*i); }
static void rect_round_assign(rect *v) { *v = rect_round(*v); }
static void v3_abs_assign(vec3 *i) { *i = v3_abs(*i); }
static void v4_abs_assign(vec4 *i) { *i = v4_abs(*i); }
static void rect_abs_assign(rect *i) { *i = rect_abs(*i); }

////////////////////////////////////////////////////////////////
//
//
// Modolu
//
//
////////////////////////////////////////////////////////////////

static ix ix_modulo(ix a, ix n) { return (a % n + n) % n; }
static i8 i8_modulo(i8 a, i8 n) { return (a % n + n) % n; }
static i16 i16_modulo(i16 a, i16 n) { return (a % n + n) % n; }
static i32 i32_modulo(i32 a, i32 n) { return (a % n + n) % n; }
static i64 i64_modulo(i64 a, i64 n) { return (a % n + n) % n; }
static ux ux_modulo(ux a, ux n) { return (a % n + n) % n; }
static u8 u8_modulo(u8 a, u8 n) { return (a % n + n) % n; }
static u16 u16_modulo(u16 a, u16 n) { return (a % n + n) % n; }
static u32 u32_modulo(u32 a, u32 n) { return (a % n + n) % n; }
static u64 u64_modulo(u64 a, u64 n) { return (a % n + n) % n; }

////////////////////////////////////////////////////////////////
//
//
// Lerp
//
//
////////////////////////////////////////////////////////////////

static f32 f32_lerp(f32 a, f32 b, f32 t) { return a + clamp01(t) * (b - a); }
static vec2 v2_lerp(vec2 a, vec2 b, f32 t) { return v2(f32_lerp(a.x, b.x, t), f32_lerp(a.y, b.y, t)); }
static vec3 v3_lerp(vec3 a, vec3 b, f32 t) { return v3(f32_lerp(a.x, b.x, t), f32_lerp(a.y, b.y, t), f32_lerp(a.z, b.z, t)); }
static vec4 v4_lerp(vec4 a, vec4 b, f32 t) { return v4(f32_lerp(a.x, b.x, t), f32_lerp(a.y, b.y, t), f32_lerp(a.z, b.z, t), f32_lerp(a.w, b.w, t)); }

static void f32_lerp_assign(f32 *a, f32 b, f32 t) { *a = f32_lerp(*a, b, t); }
static void v2_lerp_assign(vec2 *a, vec2 b, f32 t) { *a = v2_lerp(*a, b, t); }
static void v3_lerp_assign(vec3 *a, vec3 b, f32 t) { *a = v3_lerp(*a, b, t); }
static void v4_lerp_assign(vec4 *a, vec4 b, f32 t) { *a = v4_lerp(*a, b, t); }

static f32 f32_ease_out(f32 t) { return 1 - (1 - clamp01(t)) * (1 - clamp01(t)); }
static f64 f64_ease_out(f64 t) { return 1 - (1 - clamp01(t)) * (1 - clamp01(t)); }
static f32 f32_ease_in(f32 t) { return clamp01(t) * clamp01(t); }
static f64 f64_ease_in(f64 t) { return clamp01(t) * clamp01(t); }

////////////////////////////////////////////////////////////////
//
//
// Packing
//
//
////////////////////////////////////////////////////////////////

static u32 pack_u8x4(u8 u0, u8 u1, u8 u2, u8 u3) {
    return ((u32(u0) <<  0) |
            (u32(u1) <<  8) |
            (u32(u2) << 16) |
            (u32(u3) << 24));
}

static void unpack_u8x4(u32 u, u8 *u0, u8 *u1, u8 *u2, u8 *u3) {
    *u0 = u8(u >>  0);
    *u1 = u8(u >>  8);
    *u2 = u8(u >> 16);
    *u3 = u8(u >> 24);
}

static u32 pack_u16x2(u16 u0, u16 u1) {
    return ((u32(u0) <<   0) |
            (u32(u1) <<  16));
}

static void unpack_u16x2(u32 u, u16 *u0, u16 *u1) {
    *u0 = u16(u >>  0);
    *u1 = u16(u >> 16);
}

static u64 pack_u32x2(u32 a, u32 b) {
    return ((u64(a) <<  0) |
            (u64(b) << 32));
}

static void unpack_u32x2(u64 u, u32 *u0, u32 *u1) {
    *u0 = u32(u >>  0);
    *u1 = u32(u >> 32);
}

////////////////////////////////////////////////////////////////
//
//
// Bits
//
//
////////////////////////////////////////////////////////////////

static i32 first_bit_set_u32(u32 u) {
    i32 ret = -1;
    for_range(u32, i, 0, 32) {
        if (u & (1 << i)) {
            ret = i;
            break;
        }
    }
    return ret;
}

static i32 pop_first_bit_u32(u32 *u) {
    i32 ret = first_bit_set_u32(*u);
    if (ret != -1) {
        *u &= ~(1 << ret);
    }
    return ret;
}

static u32 u32_to_power_of_two(u32 u) {
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

static u64 u64_to_power_of_two(u64 u) {
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

////////////////////////////////////////////////////////////////
//
//
// Basic enums
//
//
////////////////////////////////////////////////////////////////

typedef enum { SIDE_MIN, SIDE_MAX } side;

typedef enum { AXIS2_X, AXIS2_Y } axis2;
typedef enum { AXIS3_X, AXIS3_Y, AXIS3_Z } axis3;
typedef enum { AXIS4_X, AXIS4_Y, AXIS4_Z, AXIS4_W } axis4;

typedef enum { DIR_FORWARD = 1, DIR_BACKWARD = -1, } dir;
typedef enum { DIR2_LEFT, DIR2_TOP, DIR2_RIGHT, DIR2_BOTTOM } dir2;
typedef enum { DIR3_LEFT, DIR3_TOP, DIR3_RIGHT, DIR3_BOTTOM, DIR3_UP, DIR3_DOWN } dir3;

typedef enum { CORNER_TOP_LEFT, CORNER_BOTTOM_LEFT, CORNER_TOP_RIGHT, CORNER_BOTTOM_RIGHT } corner;

typedef enum move_by {
    MOVE_BY_CHAR = 1,
    MOVE_BY_WORD,
    MOVE_BY_SUBWORD,
    MOVE_BY_LINE,
    MOVE_BY_PARAGRAPH,
    MOVE_BY_PAGE,
    MOVE_BY_HOME_END,
} move_by;

////////////////////////////////////////////////////////////////
//
//
// Any
//
//
////////////////////////////////////////////////////////////////

typedef enum any_tag {
    ANY_TAG_UNKNOWN,
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
    ANY_TAG_RANGE_IX,
    ANY_TAG_RANGE_I8,
    ANY_TAG_RANGE_I16,
    ANY_TAG_RANGE_I32,
    ANY_TAG_RANGE_I64,
    ANY_TAG_RANGE_UX,
    ANY_TAG_RANGE_U8,
    ANY_TAG_RANGE_U16,
    ANY_TAG_RANGE_U32,
    ANY_TAG_RANGE_U64,
    ANY_TAG_RANGE_F32,
    ANY_TAG_RANGE_F64,
    ANY_TAG_CHAR_PTR,
    ANY_TAG_VOID_PTR,
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
        range_ix _range_ix;
        range_i8 _range_i8;
        range_i16 _range_i16;
        range_i32 _range_i32;
        range_i64 _range_i64;
        range_ux _range_ux;
        range_u8 _range_u8;
        range_u16 _range_u16;
        range_u32 _range_u32;
        range_u64 _range_u64;
        range_f32 _range_f32;
        range_f64 _range_f64;
        char *_char_ptr;
        void *_void_ptr;
    };
};

typedef_span(any);
typedef span_any args;

#ifndef __cplusplus
#define anyof(a) (_Generic((a),                  \
        i8          : make_any_i8,               \
        i16         : make_any_i16,              \
        i32         : make_any_i32,              \
        i64         : make_any_i64,              \
        u8          : make_any_u8,               \
        u16         : make_any_u16,              \
        u32         : make_any_u32,              \
        u64         : make_any_u64,              \
        f32         : make_any_f32,              \
        f64         : make_any_f64,              \
        bool        : make_any_bool,             \
        str         : make_any_str,              \
        vec2        : make_any_vec2,             \
        vec3        : make_any_vec3,             \
        vec4        : make_any_vec4,             \
        rect        : make_any_rect,             \
        range_ix    : make_any_range_ix,         \
        range_i8    : make_any_range_i8,         \
        range_i16   : make_any_range_i16,        \
        range_i32   : make_any_range_i32,        \
        range_i64   : make_any_range_i64,        \
        range_ux    : make_any_range_ux,         \
        range_u8    : make_any_range_u8,         \
        range_u16   : make_any_range_u16,        \
        range_u32   : make_any_range_u32,        \
        range_u64   : make_any_range_u64,        \
        range_f32   : make_any_range_f32,        \
        range_f64   : make_any_range_f64,        \
        char *      : make_any_char_ptr,         \
        void *      : make_any_void_ptr          \
))((a))

#define argsof(...)    (make_span(any, ((any[]) { VA_WRAP(anyof, __VA_ARGS__) }), NUM_ARGUMENTS(__VA_ARGS__)))

static any make_any_i8(i8 a) { return lit(any) { .tag = ANY_TAG_I8, ._i8 = a }; }
static any make_any_i16(i16 a) { return lit(any) { .tag = ANY_TAG_I16, ._i16 = a }; }
static any make_any_i32(i32 a) { return lit(any) { .tag = ANY_TAG_I32, ._i32 = a }; }
static any make_any_i64(i64 a) { return lit(any) { .tag = ANY_TAG_I64, ._i64 = a }; }
static any make_any_u8(u8 a) { return lit(any) { .tag = ANY_TAG_U8, ._u8 = a }; }
static any make_any_u16(u16 a) { return lit(any) { .tag = ANY_TAG_U16, ._u16 = a }; }
static any make_any_u32(u32 a) { return lit(any) { .tag = ANY_TAG_U32, ._u32 = a }; }
static any make_any_u64(u64 a) { return lit(any) { .tag = ANY_TAG_U64, ._u64 = a }; }
static any make_any_f32(f32 a) { return lit(any) { .tag = ANY_TAG_F32, ._f32 = a }; }
static any make_any_f64(f64 a) { return lit(any) { .tag = ANY_TAG_F64, ._f64 = a }; }
static any make_any_bool(bool a) { return lit(any) { .tag = ANY_TAG_BOOL, ._bool = a }; }
static any make_any_str(str a) { return lit(any) { .tag = ANY_TAG_STR, ._str = a }; }
static any make_any_vec2(vec2 a) { return lit(any) { .tag = ANY_TAG_VEC2, ._vec2 = a }; }
static any make_any_vec3(vec3 a) { return lit(any) { .tag = ANY_TAG_VEC3, ._vec3 = a }; }
static any make_any_vec4(vec4 a) { return lit(any) { .tag = ANY_TAG_VEC4, ._vec4 = a }; }
static any make_any_rect(rect a) { return lit(any) { .tag = ANY_TAG_RECT, ._rect = a }; }
static any make_any_range_ix(range_ix a) { return lit(any) { .tag = ANY_TAG_RANGE_IX, ._range_ix = a }; }
static any make_any_range_i8(range_i8 a) { return lit(any) { .tag = ANY_TAG_RANGE_I8, ._range_i8 = a }; }
static any make_any_range_i16(range_i16 a) { return lit(any) { .tag = ANY_TAG_RANGE_I16, ._range_i16 = a }; }
static any make_any_range_i32(range_i32 a) { return lit(any) { .tag = ANY_TAG_RANGE_I32, ._range_i32 = a }; }
static any make_any_range_i64(range_i64 a) { return lit(any) { .tag = ANY_TAG_RANGE_I64, ._range_i64 = a }; }
static any make_any_range_ux(range_ux a) { return lit(any) { .tag = ANY_TAG_RANGE_UX, ._range_ux = a }; }
static any make_any_range_u8(range_u8 a) { return lit(any) { .tag = ANY_TAG_RANGE_U8, ._range_u8 = a }; }
static any make_any_range_u16(range_u16 a) { return lit(any) { .tag = ANY_TAG_RANGE_U16, ._range_u16 = a }; }
static any make_any_range_u32(range_u32 a) { return lit(any) { .tag = ANY_TAG_RANGE_U32, ._range_u32 = a }; }
static any make_any_range_u64(range_u64 a) { return lit(any) { .tag = ANY_TAG_RANGE_U64, ._range_u64 = a }; }
static any make_any_range_f32(range_f32 a) { return lit(any) { .tag = ANY_TAG_RANGE_F32, ._range_f32 = a }; }
static any make_any_range_f64(range_f64 a) { return lit(any) { .tag = ANY_TAG_RANGE_F64, ._range_f64 = a }; }
static any make_any_char_ptr(char *a) { return lit(any) { .tag = ANY_TAG_CHAR_PTR, ._char_ptr = a }; }
static any make_any_void_ptr(void *a) { return lit(any) { .tag = ANY_TAG_VOID_PTR, ._void_ptr = a }; }

#endif
