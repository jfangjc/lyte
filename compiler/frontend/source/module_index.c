#include "module_index.h"
#include "error.h"
#include "file.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define MODULE_NAME_BUF_SIZE 256

static char* duplicate_str(const char* value) {
    size_t len = strlen(value) + 1;
    char* copy = malloc(len);
    if (copy == NULL) {
        error("Out of memory");
    }
    memcpy(copy, value, len);
    return copy;
}

static void skip_ws_and_comments(const char** cursor) {
    while (**cursor != '\0') {
        if (isspace((unsigned char)**cursor)) {
            *cursor += 1;
            continue;
        }

        if (**cursor == '#') {
            while (**cursor != '\0' && **cursor != '\n') {
                *cursor += 1;
            }
            continue;
        }

        break;
    }
}

static int is_ident_start(char c) { return isalpha((unsigned char)c) || c == '_'; }

static int is_ident_continue(char c) { return isalnum((unsigned char)c) || c == '_'; }

static int scan_word(const char** cursor, const char* expected) {
    size_t len = strlen(expected);
    if (strncmp(*cursor, expected, len) != 0) {
        return 0;
    }
    if (is_ident_continue((*cursor)[len])) {
        return 0;
    }
    *cursor += len;
    return 1;
}

static void append_module_part(char* buffer, size_t buffer_size, size_t* length, const char* start, size_t part_len) {
    if (*length + part_len + 1 >= buffer_size) {
        error("Module name is too long.");
    }

    memcpy(buffer + *length, start, part_len);
    *length += part_len;
    buffer[*length] = '\0';
}

static char* get_module_name(const char* path) {
    char* source = read_file_contents(path);
    const char* cursor = source;
    char name[MODULE_NAME_BUF_SIZE];
    size_t name_len = 0;

    name[0] = '\0';
    skip_ws_and_comments(&cursor);

    if (!scan_word(&cursor, "module")) {
        free(source);
        error("Expected module declaration before other declarations.");
    }

    skip_ws_and_comments(&cursor);
    while (1) {
        const char* part_start = cursor;
        if (!is_ident_start(*cursor)) {
            free(source);
            error("Expected module name.");
        }

        cursor += 1;
        while (is_ident_continue(*cursor)) {
            cursor += 1;
        }

        append_module_part(name, sizeof(name), &name_len, part_start, (size_t)(cursor - part_start));

        skip_ws_and_comments(&cursor);
        if (*cursor != '.') {
            break;
        }

        append_module_part(name, sizeof(name), &name_len, ".", 1);
        cursor += 1;
        skip_ws_and_comments(&cursor);
    }

    free(source);
    return duplicate_str(name);
}

static struct module_group* find_module(struct module_groups* groups, const char* name) {
    struct module_group* group = groups->modules;
    while (group != NULL) {
        if (strcmp(group->name, name) == 0) {
            return group;
        }
        group = group->next;
    }
    return NULL;
}

static struct module_group* add_module(struct module_groups* groups, const char* name) {
    struct module_group* group = malloc(sizeof(struct module_group));
    if (group == NULL) {
        error("Out of memory");
    }

    group->name = duplicate_str(name);
    group->files = NULL;
    group->next = groups->modules;
    groups->modules = group;
    return group;
}

static void add_file_to_module(struct module_group* group, const char* path) {
    struct module_file* file = malloc(sizeof(struct module_file));
    if (file == NULL) {
        error("Out of memory");
    }

    file->path = duplicate_str(path);
    file->next = NULL;

    if (group->files == NULL) {
        group->files = file;
        return;
    }

    struct module_file* curr = group->files;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = file;
}

void module_index_init(struct module_groups* groups, int file_count, char** paths) {
    groups->modules = NULL;

    for (int i = 0; i < file_count; i++) {
        char* module_name = get_module_name(paths[i]);
        struct module_group* group = find_module(groups, module_name);

        if (group == NULL) {
            group = add_module(groups, module_name);
        }

        add_file_to_module(group, paths[i]);
        free(module_name);
    }
}

void module_index_free(struct module_groups* groups) {
    struct module_group* group = groups->modules;
    while (group != NULL) {
        struct module_group* next_group = group->next;
        struct module_file* file = group->files;

        while (file != NULL) {
            struct module_file* next_file = file->next;
            free(file->path);
            free(file);
            file = next_file;
        }

        free(group->name);
        free(group);
        group = next_group;
    }

    groups->modules = NULL;
}
