#ifndef COMPILER_MODULE_INDEX
#define COMPILER_MODULE_INDEX

struct module_file {
    char* path;
    struct module_file* next;
};

struct module_group {
    char* name;
    struct module_file* files;
    struct module_group* next;
};

struct module_groups {
    struct module_group* modules;
};

void module_index_init(struct module_groups* groups, int file_count, char** paths);
void module_index_free(struct module_groups* groups);

#endif
