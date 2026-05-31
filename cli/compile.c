#include "compile.h"

#include "codegen.h"
#include "error.h"
#include "lexer.h"
#include "module_ast_builder.h"
#include "module_index.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void output_path_from_input(const char* path, char* output_path, size_t output_size) {
    size_t len = strlen(path);
    if (len >= output_size) {
        error("Output path is too long");
    }
    if (len < 3 || path[len - 3] != '.' || path[len - 2] != 'l' || path[len - 1] != 't') {
        error("Expected input file with .lt extension");
    }

    memcpy(output_path, path, len + 1);
    output_path[len - 2] = 's';
    output_path[len - 1] = '\0';
}

static void compile_module(struct module_group* module) {
    if (module->files == NULL) {
        return;
    }

    struct module_ast_builder builder;
    module_ast_builder_init(&builder, module->name);

    struct module_file* file = module->files;
    while (file != NULL) {
        read_file(file->path);
        struct program_ast* file_program = parse_program();

        if (file_program->module_path != NULL && strcmp(file_program->module_path, module->name) != 0) {
            error("Module index and parsed module name differ");
        }

        module_ast_builder_append(&builder, file_program);
        free(file_program);
        file = file->next;
    }

    char output_path[1024];
    output_path_from_input(module->files->path, output_path, sizeof(output_path));

    module_ast_validate_exports(&builder.program);

    printf("%s", output_path);
    gen(output_path, &builder.program);
}

void compile_files(int file_count, char** paths) {
    if (file_count < 1) {
        error("No input files");
    }

    struct module_groups groups;
    module_index_init(&groups, file_count, paths);

    struct module_group* module = groups.modules;
    while (module != NULL) {
        compile_module(module);
        module = module->next;
    }

    module_index_free(&groups);
}
