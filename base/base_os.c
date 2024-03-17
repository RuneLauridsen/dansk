////////////////////////////////////////////////////////////////
//
//
// Entire file
//
//
////////////////////////////////////////////////////////////////

static bool os_win32_valid_handle(HANDLE handle) {
    bool ret = (handle && handle != INVALID_HANDLE_VALUE);
    return ret;
}

// @Todo Larger than 4gb files.
static str os_read_entire_file(str file_name, arena *arena) {
    str ret = { 0 };

    HANDLE file = INVALID_HANDLE_VALUE;
    arena_temp(arena) {
        wstr wname = convert_utf8_to_utf16(file_name, arena);
        file = CreateFileW((LPWSTR)wname.v, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    }

    if (os_win32_valid_handle(file)) {
        u32 file_size = GetFileSize(file, 0);
        ret.v = arena_push_size_nz(arena, file_size + 1, 0);
        if (ret.v) {
            ret.v[0] = '\0';

            DWORD bytes_read = 0;
            if (ReadFile(file, ret.v, file_size, &bytes_read, 0)) {
                // @Todo Larger than 4gb files.
                // @Todo Return error.
                assert(bytes_read == file_size);

                ret.count         = bytes_read;
                ret.v[bytes_read] = '\0';
            } else {
                println("ReadFile failed (%).", (u32)GetLastError());
            }
        }

        CloseHandle(file);
    } else {
        println("CreateFileW failed (%).", (u32)GetLastError());
    }

    return ret;
}

// @Todo Larger than 4gb files.
static bool os_write_entire_file(str file_name, str data, arena *arena) {
    bool ret = false;

    HANDLE file = INVALID_HANDLE_VALUE;
    arena_temp(arena) {
        wstr wname = convert_utf8_to_utf16(file_name, arena);
        file = CreateFileW((LPWSTR)wname.v, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    }

    if (os_win32_valid_handle(file)) {
        DWORD bytes_written = 0;
        if (WriteFile(file, data.v, (DWORD)data.len, &bytes_written, 0)) {
            // @Todo Larger than 4gb files.
            if (bytes_written == (DWORD)data.len) {
                ret = true;
            }
        } else {
            println("WriteFile failed (%).", (u32)GetLastError());
        }

        CloseHandle(file);
    } else {
        println("CreateFileW failed (%).", (u32)GetLastError());
    }

    return ret;

}

////////////////////////////////////////////////////////////////
//
//
// File iterator
//
//
////////////////////////////////////////////////////////////////

static bool os_is_path_seperator(u32 c) {
    bool ret = ((c == '/') || (c == '\\'));
    return ret;
}

static u32 os_get_logical_drives(void) {
    return GetLogicalDrives();
}

static void os_file_iter_begin(os_file_iter *iter, str path, arena *temp) {
    zero(iter);
    arena_temp(temp) {
        wstr wpath = convert_utf8_to_utf16(path, temp);
        if (wpath.count) {
            iter->find_handle = FindFirstFileW((LPWSTR)wpath.v, &iter->find_data);

            if (!os_win32_valid_handle(iter->find_handle)) {
                //println("FindFirstFileW failed (%).", (u32)GetLastError());
            }
        }
    }
}

static bool os_file_iter_next(os_file_iter *iter, os_file_info *out, arena *arena) {
    b32 ret = false;

    //
    // NOTE(rune): Find next file but skip "." and "..".
    //

    if (os_win32_valid_handle(iter->find_handle)) {
        while (1) {
            if (iter->count == 0) {
                ret = true; // NOTE(rune): First result came from FindFirstFileW().
            } else {
                ret = FindNextFileW(iter->find_handle, &iter->find_data);
            }

            if (!ret) break;

            iter->count++;

            bool skip = false;

            if (iter->find_data.cFileName[0] == '\0') {
                skip = true;
            } else if ((iter->find_data.cFileName[0] == '.') &&
                       (iter->find_data.cFileName[1] == '\0')) {
                skip = true;
            } else if ((iter->find_data.cFileName[0] == '.') &&
                       (iter->find_data.cFileName[1] == '.') &&
                       (iter->find_data.cFileName[2] == '\0')) {
                skip = true;
            }

            if (!skip) break;
        }
    }

    //
    // NOTE(rune): Convert utf16 filepath to utf8.
    //

    if (ret) {

        wstr wpath = { 0 };
        wpath.v = iter->find_data.cFileName;

        for_n (i32, i, countof(iter->find_data.cFileName)) {
            if (iter->find_data.cFileName[i] == '\0') {
                break;
            }

            wpath.count++;
        }

        out->name = convert_utf16_to_utf8(wpath, arena);
        out->is_directory = iter->find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
        out->is_file = !out->is_directory;
    } else {
        zero(out);
    }

    return ret;
}

static void os_file_iter_end(os_file_iter *iter) {
    if (os_win32_valid_handle(iter->find_handle)) {
        FindClose(iter->find_handle);
    }
}

static os_file_infos os_get_files_in_path(str path, i32 max, arena *arena) {
    if (max == 0) max = 0xffff'ffff;

    os_file_infos ret = { 0 };
    os_file_info info;
    os_file_iter iter;
    os_file_iter_begin(&iter, path, arena);
    while (ret.count < max && os_file_iter_next(&iter, &info, arena)) {
        os_file_info *node = arena_push_struct(arena, os_file_info);
        *node = info;

        slist_add(&ret, node);
        ret.count++;
    }
    os_file_iter_end(&iter);

    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Performance counter
//
//
////////////////////////////////////////////////////////////////

static u64 os_get_performance_timestamp(void) {
    u64 ret = 0;
    QueryPerformanceCounter(cast(LARGE_INTEGER *, &ret));
    return ret;
}

static f64 os_get_millis_between(u64 t_begin, u64 t_end) {
    u64 diff = t_end - t_begin;
    u64 freq = 1;
    QueryPerformanceFrequency(cast(LARGE_INTEGER *, &freq));
    f64 ret = 1000.0 * f64(diff) / f64(freq);
    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Prof scope
//
//
////////////////////////////////////////////////////////////////

typedef struct prof_scope prof_scope;
struct prof_scope {
    bool init;
    u64 t_begin;
    u64 t_end;
};

static inline bool prof_scope_func(prof_scope *scope, char *name) {
    if (!scope->init) {
        scope->t_begin = os_get_performance_timestamp();
        scope->init = true;
        return true;
    } else {
        scope->t_end = os_get_performance_timestamp();
        println("% us \t %", os_get_millis_between(scope->t_begin, scope->t_end) * 1000.0f, name);
        return false;
    }
}

#ifdef NDEBUG
#define prof_scope(name) for(prof_scope _scope_ = {0}; prof_scope_func(&_scope_, name);)
#else
#define prof_scope(...)
#endif
