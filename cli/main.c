#include <stdio.h>
#include <string.h>

#include "codegen.h"
#include "error.h"
#include "parser.h"
#include "lexer.h"
#include "module_index.h"

static void compile_file(char* path) {
    read_file(path);
    struct program_ast* program = parse_program();

    size_t len = strlen(path);
    char output_path[1024];
    if (len >= sizeof(output_path)) {
        error("Output path is too long");
    }
    if (len < 3 || path[len - 3] != '.' || path[len - 2] != 'l' ||
        path[len - 1] != 't') {
        error("Expected input file with .lt extension");
    }

    memcpy(output_path, path, len + 1);
    output_path[len - 2] = 's';
    output_path[len - 1] = '\0';

    printf("%s", output_path);
    gen(output_path, program);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        error("No input files");
    }

    struct module_groups groups;
    module_index_init(&groups, argc - 1, argv + 1);

    struct module_group* module = groups.modules;
    while (module != NULL) {
        struct module_file* file = module->files;
        while (file != NULL) {
            compile_file(file->path);
            file = file->next;
        }
        module = module->next;
    }

    module_index_free(&groups);

    printf("Compilation finished\n");
    return 0;
}
