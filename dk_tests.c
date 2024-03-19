////////////////////////////////////////////////////////////////
//
//
// Assertions
//
//
////////////////////////////////////////////////////////////////

static void dk_test_assert_true(str loc, str expression, bool b) {
    if (b == false) {
        dk_global_test_ok = false;

        print(ANSICONSOLE_FG_BRIGHT_RED);
        println("");
        println("Assertion failed: %", loc);
        println("Expression:       %", expression);
        print(ANSICONSOLE_FG_DEFAULT);
    }
}

static void dk_test_assert_eq_any(str loc, any actual, any expect) {
    if (any_eq(expect, actual) == false) {
        dk_global_test_ok = false;

        print(ANSICONSOLE_FG_BRIGHT_RED);
        println("");
        println("Assertion failed: %", loc);
        println("Expected:         %(literal)", expect);
        println("Actual:           %(literal)", actual);
        print(ANSICONSOLE_FG_DEFAULT);
    }
}

static void dk_test_assert_program_output(str loc, str source_code, str expect_output) {
    str actual_output = { 0 };

    dk_err_sink sink = { 0 };
    dk_program program = dk_program_from_str(source_code, &sink);

    if (sink.err_list.count > 0) {
        // TODO(rune): When asserting error output, we dont want to match on the error message string,
        // since it's annoying the fix the tests, every time a change is made to the error reporting.
        actual_output = dk_str_from_err_sink(&sink, &dk_global_test_arena);
    } else {
        actual_output = dk_run_program(program, &dk_global_test_arena);
    }

    dk_test_assert_eq(loc, actual_output, expect_output);
}

////////////////////////////////////////////////////////////////
//
//
// Test file parsing
//
//
////////////////////////////////////////////////////////////////

static dk_tests dk_tests_from_file(str file_path, arena *arena) {
    dk_tests tests = { 0 };

    //- Read file.
    str file_data = os_read_entire_file(file_path, arena);

    //- Split into sections.
    str_list sections = str_split_by_delim(file_data, str("════════════════════════════════════════════════════════════════\n"), arena);
    for_list (str_node, node, sections) {
        //- Parse section.
        str section = node->v;
        str_list section_parts = str_split_by_delim(section, str("────────────────────────────────────────────────────────────────\n"), arena);
        if (section_parts.count >= 3) {
            str part1 = section_parts.first->v;
            str part2 = section_parts.first->next->v;
            str part3 = section_parts.first->next->next->v;

            //- Add to list of tests.
            dk_test *t   = arena_push_struct(arena, dk_test);
            t->file_path = file_path;
            t->name      = str_trim(part1);
            t->input     = part2;
            t->output    = part3;

            slist_add(&tests, t);
            tests.count += 1;
        } else {
            //- Could not parse.
            print(ANSICONSOLE_FG_RED);
            println("Invalid test file format.");
            println("File path: %", file_path);
            println("Byte pos:  %", section.v - file_data.v);
            print(ANSICONSOLE_FG_DEFAULT);
        }
    }

    return tests;
}

////////////////////////////////////////////////////////////////
//
//
// Runner
//
//
////////////////////////////////////////////////////////////////

static void dk_test_begin(void) {
    dk_global_test_ok = true;
    dk_global_test_timestamp_begin = os_get_performance_timestamp();
}

static void dk_test_end(str test_name) {
    dk_global_test_timestamp_end = os_get_performance_timestamp();
    f64 ms = os_get_millis_between(dk_global_test_timestamp_begin, dk_global_test_timestamp_end);

    arena_free(&dk_global_test_arena);

    print(ANSICONSOLE_FG_DEFAULT "%", test_name);
    if (dk_global_test_ok) {
        print(ANSICONSOLE_FG_GREEN " (PASS)");
    } else {
        print(ANSICONSOLE_FG_RED " (FAIL)");
    }
    print(ANSICONSOLE_FG_GRAY " % ms", ms);
    print(ANSICONSOLE_FG_DEFAULT "\n");
}

static void dk_run_test_file_filtered(str file_path, str filter) {
    arena arena = { 0 };

    //- Parse test file.
    dk_tests tests = dk_tests_from_file(file_path, &arena);

    //- Loop over tests in file.
    for_list (dk_test, t, tests) {
        if (str_idx_of_str(t->name, filter).found) {
            dk_test_begin();
            dk_test_assert_program_output(loc(), t->input, t->output);

            print(ANSICONSOLE_FG_GRAY);
            print("% ", t->file_path);
            print(ANSICONSOLE_FG_DEFAULT);
            dk_test_end(t->name);
        }
    }

    //- Cleanup.
    arena_free(&arena);
}


static void dk_run_test_file(str file_name) {
    dk_run_test_file_filtered(file_name, str(""));
}

static void dk_run_all_tests(void) {
    dk_run_test_file(str("W:\\dansk\\tests\\tests.dk")); // TODO(rune): Hardcoded path.
}