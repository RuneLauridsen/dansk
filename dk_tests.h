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
static void dk_run_test_file_filtered(str file_path, str filter, arena *arena);
static void dk_run_test_file(str file_name, arena *arena);

//- All tests.
static void dk_run_all_tests(void);