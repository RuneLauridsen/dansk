#include "base/base.h"

arena temp_arena = { 0 };

#define tprint(...) tprint_args(argsof(__VA_ARGS__))
static str tprint_args(args args) {
    str ret = push_print_args(&temp_arena, args);
    return ret;
}

#include "dansk.h"
#include "dansk.c"
#include "dansk_tokenizer.h"
#include "dansk_tokenizer.c"
#include "dansk_ast.h"
#include "dansk_ast.c"
#include "dansk_parser.h"
#include "dansk_parser.c"
#include "dansk_interp.h"
#include "dansk_interp.c"

int main(int argc, char **argv) {
    if (argc < 2) {
        println("Usage: % <program.dk>", argv[0]);
        exit(0);
    }

    SetConsoleOutputCP(65001); // utf8

    str file_name = str_from_cstr(argv[1]);

    arena file_arena = { 0 };
    arena token_arena = { 0 };
    arena ast_arena = { 0 };
    arena interp_arena = { 0 };

    str source_code = os_read_entire_file(file_name, &file_arena);
    if (!source_code.len) {
        println("Could not read file '%'.", file_name);
        exit(0);
    }

    tokens tokens = tokenize(source_code, &token_arena);
    print_tokens(tokens);

    ast ast = parse(tokens, &ast_arena);
    print_ast(ast);

    interp interp = { 0 };
    interp_init(&interp, ast, &interp_arena);
    while (interp.next_node) {
        interp_exec_next_node(&interp);
    }

    println("Used % bytes for file.", arena_size_used(&file_arena));
    println("Used % bytes for tokens.", arena_size_used(&token_arena));
    println("Used % bytes for ast.", arena_size_used(&ast_arena));
    println("Used % bytes for interp.", arena_size_used(&interp_arena));

    free_arena(&file_arena);
    free_arena(&token_arena);
    free_arena(&ast_arena);
    free_arena(&interp_arena);
}
