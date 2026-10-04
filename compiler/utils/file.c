#include "file.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>

char* read_file_contents(const char* path) {
    FILE* file = NULL;
#ifdef _MSC_VER
    fopen_s(&file, path, "rb");
#else
    file = fopen(path, "rb");
#endif

    if (file == NULL) {
        error("File does not exist.");
    }

    fseek(file, 0, SEEK_END);
    long raw_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (raw_size < 0) {
        fclose(file);
        error("Unable to read file.");
    }

    size_t size = (size_t)raw_size;
    char* content = malloc(size + 1);
    if (content == NULL) {
        fclose(file);
        error("Out of memory");
    }

    if (size > 0) {
        size_t read_count = fread(content, 1, size, file);
        if (read_count != size) {
            free(content);
            fclose(file);
            error("Unable to read file.");
        }
    }

    content[size] = '\0';
    fclose(file);
    return content;
}
