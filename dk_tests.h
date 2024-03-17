////////////////////////////////////////////////////////////////
//
//
// Global
//
//
////////////////////////////////////////////////////////////////

//- Global variables.
static bool dk_global_test_ok;
static arena dk_global_test_arena;
static u64 dk_global_test_timestamp_begin;
static u64 dk_global_test_timestamp_end;

//- Global state managment.
static void dk_test_begin(void);
static void dk_test_end(str test_name);

////////////////////////////////////////////////////////////////
//
//
// Assertions
//
//
////////////////////////////////////////////////////////////////

#define     dk_test_assert(loc, expression)        (dk_test_assert_true((loc), str(#expression), (expression)))
#define     dk_test_assert_eq(loc, actual, expect) (dk_test_assert_eq_any((loc), anyof(actual), anyof(expect)))
static void dk_test_assert_true(str loc, str expression, bool b);
static void dk_test_assert_eq_any(str loc, any actual, any expect);
static void dk_test_assert_program_output(str loc, str source_code, str expect_output);

////////////////////////////////////////////////////////////////
//
//
// Test file parsing
//
//
////////////////////////////////////////////////////////////////

typedef struct dk_test dk_test;
struct dk_test {
    str file_path;
    str name;
    str input;
    str output;
    dk_test *next;
};

typedef struct dk_tests dk_tests;
struct dk_tests {
    dk_test *first;
    dk_test *last;
    u64 count;
};

static dk_tests dk_tests_from_file(str file_path, arena *arena);

////////////////////////////////////////////////////////////////
//
//
// Runner
//
//
////////////////////////////////////////////////////////////////

//- Run test from parsed file.
static void dk_run_test_file_filtered(str file_path, str filter);
static void dk_run_test_file(str file_name);

//- All tests.
static void dk_run_all_tests(void);