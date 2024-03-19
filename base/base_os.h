////////////////////////////////////////////////////////////////
//
//
// Entire file
//
//
////////////////////////////////////////////////////////////////

static str os_read_entire_file(str file_name, arena *arena);
static bool os_write_entire_file(str file_name, str data, arena *arena);

////////////////////////////////////////////////////////////////
//
//
// File iterator
//
//
////////////////////////////////////////////////////////////////

typedef struct os_file_info os_file_info;
struct os_file_info {
    str name;
    bool is_directory;
    bool is_drive;
    bool is_file;
    os_file_info *next;
};

typedef struct os_file_infos os_file_infos;
struct os_file_infos {
    os_file_info *first;
    os_file_info *last;
    u64 count;
};

typedef struct os_file_iter os_file_iter;
struct os_file_iter {
    HANDLE find_handle;
    WIN32_FIND_DATAW find_data;
    u64 count;
};

static void os_file_iter_begin(os_file_iter *iter, str path, arena *temp);
static bool os_file_iter_next(os_file_iter *iter, os_file_info *out, arena *arena);
static void os_file_iter_end(os_file_iter *iter);

////////////////////////////////////////////////////////////////
//
//
// Directories
//
//
////////////////////////////////////////////////////////////////

static bool os_is_path_seperator(u32 c);
static os_file_infos os_get_files_from_path(str path, u64 max, arena *arena);

////////////////////////////////////////////////////////////////
//
//
// Performance counter
//
//
////////////////////////////////////////////////////////////////

static u64 os_get_performance_timestamp(void);
static f64 os_get_millis_between(u64 t_begin, u64 t_end);
