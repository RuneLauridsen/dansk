////////////////////////////////////////////////////////////////
//
//
// Print
//
//
////////////////////////////////////////////////////////////////

static ix sprint_args(u8 *out, ix cap, args args);
static void print_args(args args);
static void println_args(args args);
static str push_print_args(arena *arena, args args);

#define sprint(out, cap, ...)   sprint_args(out, cap, argsof(__VA_ARGS)))
#define print(...)              print_args(argsof(__VA_ARGS__))
#define println(...)            println_args(argsof(__VA_ARGS__))
#define push_print(arena, ...)  push_print_args(arena, argsof(__VA_ARGS__))

#define dumpvar(expr) println("% = %var", #expr, (expr))
