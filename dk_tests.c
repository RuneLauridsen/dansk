////////////////////////////////////////////////////////////////
//
//
// Test file parsing
//
//
////////////////////////////////////////////////////////////////

static str dk_lf_normalize(str s, arena *arena) {
    str normalized = { arena_push_array(arena, u8, s.len) };
    u64 i = 0;
    u64 j = 0;
    while (i < s.len) {
        u8 c0 = i + 0 < s.len ? s.v[i + 0] : '\0';
        u8 c1 = i + 1 < s.len ? s.v[i + 1] : '\0';

        if (c0 == '\r' && c1 == '\n') {
            i++;
        } else {
            normalized.v[j++] = s.v[i++];
        }
    }
    normalized.len = j;
    return normalized;
}

static dk_tests dk_tests_from_file(str file_path, arena *arena) {
    dk_tests tests = { 0 };

    //- Read file.
    str file_data = os_read_entire_file(file_path, arena);
    file_data = dk_lf_normalize(file_data, arena);

    //- Split into sections.
    str_list sections = str_split_by_delim(file_data, str("════════════════════════════════════════════════════════════════"), arena);
    for_list (str_node, node, sections) {
        //- Parse section.
        str section = str_trim(node->v);
        str_list section_parts = str_split_by_delim(section, str("────────────────────────────────────────────────────────────────"), arena);
        if (section_parts.count >= 3) {
            str part1 = str_trim(section_parts.first->v);
            str part2 = str_trim(section_parts.first->next->v);
            str part3 = str_trim(section_parts.first->next->next->v);

            //- Add to list of tests.
            dk_test *t   = arena_push_struct(arena, dk_test);
            t->file_path = file_path;
            t->name      = part1;
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

static void dk_run_test_file_filtered(str file_path, str filter, arena *arena) {
    //- Parse test file.
    dk_tests tests = dk_tests_from_file(file_path, arena);

    //- Loop over tests in file.
    for_list (dk_test, test, tests) {
        arena_begin_temp(arena);
        if (str_idx_of_str(test->name, filter).found) {
            //- Run test.
            str actual_output = { 0 };
            f64 elapsed_millis = 0.0;
            {
                u64 timestamp_begin = os_get_performance_timestamp();

                dk_err_sink err_sink = { 0 };
                dk_program program = dk_program_from_str(test->input, &err_sink);

                if (err_sink.err_list.count > 0) {
                    // TODO(rune): When asserting error output, we dont want to match on the error message string,
                    // since it's annoying the fix the tests, every time a change is made to the error reporting.
                    actual_output = dk_str_from_err_sink(&err_sink, arena);
                } else {
                    actual_output = dk_run_program(program, arena);
                }

                u64 timestamp_end = os_get_performance_timestamp();
                elapsed_millis= os_get_millis_between(timestamp_begin, timestamp_end);
            }

            //- Check result. We don't care about whitespace.
            actual_output     = str_trim(actual_output);
            str expect_output = str_trim(test->output);
            bool succeeded    = str_eq(actual_output, expect_output);

            //- Print result.
            print(ANSICONSOLE_FG_GRAY    "% ", test->file_path);
            print(ANSICONSOLE_FG_DEFAULT "% ", test->name);
            if (succeeded) {
                print(ANSICONSOLE_FG_GREEN "(PASS) ");
            } else {
                print(ANSICONSOLE_FG_RED "(FAIL) ");
            }
            print(ANSICONSOLE_FG_GRAY "% ms ", elapsed_millis);
            print(ANSICONSOLE_FG_DEFAULT "\n");

            //- Print expected/actual diff.
            if (succeeded == false) {
                print(ANSICONSOLE_FG_BRIGHT_RED);
                println("Expected:         %(literal)", expect_output);
                println("Actual:           %(literal)", actual_output);
                print(ANSICONSOLE_FG_DEFAULT);
            }
        }

        arena_end_temp(arena);
    }
}

static void dk_run_test_file(str file_name, arena *arena) {
    dk_run_test_file_filtered(file_name, str(""), arena);
}

// NOTE(rune): For debugging.
static void dk_run_all_tests(void) {
    arena arena = { 0 };

    dk_run_test_file(str("W:\\dansk\\tests\\tests.dk"), &arena); // TODO(rune): Hardcoded path.
    dk_run_test_file(str("W:\\dansk\\tests\\test_typecheck.dk"), &arena); // TODO(rune): Hardcoded path.

    arena_free(&arena);
}