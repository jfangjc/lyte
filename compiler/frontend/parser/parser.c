#include "common.h"
#include "error.h"
#include "parser_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct fn_decl* parse_fn_decl(int is_exported, int is_unsafe);
static struct fn_decl* parse_fn_decl_with_optional_unsafe(int is_exported);
static void parse_module_decl(struct program_ast* program);
static struct import_decl* parse_import_decl(void);
static struct export_decl* parse_export_block(void);
static void parse_exported_decl(struct fn_decl*** fn, struct export_decl*** export, struct type_decl*** type,
                                struct var_decl*** var);
static struct type_decl* parse_type_decl(void);
static int is_top_level_start(void);

struct token* curr_token;

void next(void) {
    curr_token = next_token();
}

void expect(int type) {
    if (curr_token == NULL) {
        char error_str[256];
        snprintf(error_str, 256, "\n ERROR: expect %c, get NULL \n", type);
        error(error_str);
        return;
    }
    if (curr_token->type == type) {
        next();
        return;
    }
    char error_str[256];
    snprintf(error_str, 256, "\n ERROR: expect %c, get %c \n", type, curr_token->start_pos[0]);
    error(error_str);
    return;
}

int match(int type) { return curr_token->type == type; }

static void init_program_ast(struct program_ast* program) {
    program->module_name = NULL;
    program->module_path = NULL;
    program->imports = NULL;
    program->exports = NULL;
    program->type_decls = NULL;
    program->var_decls = NULL;
    program->fn_decls = NULL;
}

static void append_name_part(char** buffer, size_t* length, size_t* capacity, const char* text, size_t text_length) {
    size_t needed = *length + text_length + 1;
    if (needed > *capacity) {
        size_t new_capacity = *capacity == 0 ? 32 : *capacity;
        while (needed > new_capacity) {
            new_capacity *= 2;
        }

        char* new_buffer = realloc(*buffer, new_capacity);
        if (new_buffer == NULL) {
            error("Out of memory");
        }
        *buffer = new_buffer;
        *capacity = new_capacity;
    }

    memcpy(*buffer + *length, text, text_length);
    *length += text_length;
    (*buffer)[*length] = '\0';
}

char* parse_qualified_name(struct token** first_token) {
    struct token* id = parse_id();
    if (first_token != NULL) {
        *first_token = id;
    }

    char* name = NULL;
    size_t length = 0;
    size_t capacity = 0;
    append_name_part(&name, &length, &capacity, id->start_pos, (size_t)id->length);

    while (match('.')) {
        next();
        id = parse_id();
        append_name_part(&name, &length, &capacity, ".", 1);
        append_name_part(&name, &length, &capacity, id->start_pos, (size_t)id->length);
    }

    return name;
}

// Program
struct program_ast* parse_program(void) {
    struct program_ast* program = malloc(sizeof(struct program_ast));
    struct fn_decl** fn = &(program->fn_decls);
    struct import_decl** import = &(program->imports);
    struct export_decl** export = &(program->exports);
    struct type_decl** type = &(program->type_decls);
    struct var_decl** var = &(program->var_decls);

    init_program_ast(program);
    curr_token = next_token();

    while (curr_token != NULL) {
        if (match(TOK_MODULE)) {
            parse_module_decl(program);
        }
        else if (match(TOK_IMPORT)) {
            *import = parse_import_decl();
            import = &((*import)->next);
        }
        else if (match(TOK_EXPORT)) {
            parse_exported_decl(&fn, &export, &type, &var);
        }
        else if (match(TOK_TYPE)) {
            *type = parse_type_decl();
            type = &((*type)->next);
        }
        else if (match(TOK_VAR) || match(TOK_LET)) {
            *var = parse_var_decl();
            var = &((*var)->next);
        }
        else if (match(TOK_UNSAFE)) {
            *fn = parse_fn_decl_with_optional_unsafe(0);
            fn = &((*fn)->next);
        }
        else if (match(TOK_FN)) {
            (*fn) = parse_fn_decl_with_optional_unsafe(0);
            fn = &((*fn)->next);
        }
        else if (match('@')) {
            next();
            parse_id();
        }
        else {
            error("Expected top-level declaration");
        }
    }
    (*fn) = NULL;
    (*import) = NULL;
    (*export) = NULL;
    (*type) = NULL;
    (*var) = NULL;
    return program;
}

// Declarations
static struct fn_decl* parse_fn_decl_with_optional_unsafe(int is_exported) {
    int is_unsafe = 0;
    if (match(TOK_UNSAFE)) {
        is_unsafe = 1;
        next();
    }
    return parse_fn_decl(is_exported, is_unsafe);
}

static struct fn_decl* parse_fn_decl(int is_exported, int is_unsafe) {
    expect(TOK_FN);

    struct fn_decl* fn = malloc(sizeof(struct fn_decl));

    fn->name = parse_id();
    fn->params = parse_param_list();

    expect(':');

    fn->type = parse_type();
    fn->body = parse_compound_stmt();
    fn->is_exported = is_exported;
    fn->is_unsafe = is_unsafe;
    fn->next = NULL;

    return fn;
}

static void parse_module_decl(struct program_ast* program) {
    expect(TOK_MODULE);
    program->module_path = parse_qualified_name(&program->module_name);
}

static struct import_decl* parse_import_decl(void) {
    struct import_decl* import = malloc(sizeof(struct import_decl));
    expect(TOK_IMPORT);

    import->module_path = parse_qualified_name(&import->module_name);

    import->alias = NULL;
    if (match(TOK_AS)) {
        next();
        import->alias = parse_id();
    }
    import->next = NULL;
    return import;
}

static void append_export_block(struct export_decl*** export, struct export_decl* block) {
    **export = block;
    while (**export != NULL) {
        *export = &((**export)->next);
    }
}

static void parse_exported_decl(struct fn_decl*** fn, struct export_decl*** export, struct type_decl*** type,
                                struct var_decl*** var) {
    expect(TOK_EXPORT);

    if (match('{')) {
        append_export_block(export, parse_export_block());
    }
    else if (match(TOK_TYPE)) {
        **type = parse_type_decl();
        (**type)->is_exported = 1;
        *type = &((**type)->next);
    }
    else if (match(TOK_UNSAFE) || match(TOK_FN)) {
        **fn = parse_fn_decl_with_optional_unsafe(1);
        *fn = &((**fn)->next);
    }
    else if (match(TOK_VAR) || match(TOK_LET)) {
        **var = parse_var_decl();
        (**var)->is_exported = 1;
        *var = &((**var)->next);
    }
    else {
        error("Expected declaration after export");
    }
}

static struct export_decl* parse_export_block(void) {
    expect('{');

    struct export_decl* head = NULL;
    struct export_decl** curr = &head;
    while (!match('}')) {
        *curr = malloc(sizeof(struct export_decl));
        (*curr)->name = parse_id();
        (*curr)->next = NULL;
        curr = &((*curr)->next);
        if (match(',')) {
            next();
        }
    }
    expect('}');
    return head;
}

static struct type_decl* parse_type_decl(void) {
    struct type_decl* type = malloc(sizeof(struct type_decl));
    expect(TOK_TYPE);
    type->name = parse_id();
    type->alias = NULL;
    type->is_exported = 0;

    if (match(TOK_ASSIGN)) {
        next();
        if (match('{')) {
            int depth = 0;
            do {
                if (match('{')) {
                    depth++;
                }
                else if (match('}')) {
                    depth--;
                }
                next();
            } while (curr_token != NULL && depth > 0);
        }
        else if (match(TOK_ID) || match('*') || match('&')) {
            type->alias = parse_type();
        }
        else {
            while (curr_token != NULL && !is_top_level_start()) {
                next();
            }
        }
    }
    type->next = NULL;
    return type;
}

static int is_top_level_start(void) {
    return match(TOK_MODULE) || match(TOK_IMPORT) || match(TOK_EXPORT) || match(TOK_TYPE) || match(TOK_FN) ||
           match(TOK_LET) || match(TOK_VAR) || match(TOK_UNSAFE) || match('@');
}

struct var_decl* parse_var_decl(void) {
    struct var_decl* var = malloc(sizeof(struct var_decl));

    var->is_const = match(TOK_LET);
    var->is_exported = 0;
    if (var->is_const) {
        expect(TOK_LET);
    }
    else {
        expect(TOK_VAR);
    }

    var->name = parse_id();

    if (match(':')) {
        next();
        var->type = parse_type();
    }
    else {
        var->type = NULL; // NULL for unknown type
    }

    var->value = parse_initialiser();
    var->next = NULL;

    expect(';');
    return var;
}

// Parameter list
struct param* parse_param_list(void) {
    expect('(');

    if (!match(')')) {
        struct param* param = malloc(sizeof(struct param));
        struct param* head = param;

        if (match(TOK_ID) && curr_token->length == 3 && strncmp(curr_token->start_pos, "out", 3) == 0) {
            next();
        }
        param->name = parse_id();
        expect(':');
        param->type = parse_type();

        while (match(',')) {
            param->next = malloc(sizeof(struct param));
            param = param->next;
            next();

            if (match(TOK_ID) && curr_token->length == 3 && strncmp(curr_token->start_pos, "out", 3) == 0) {
                next();
            }
            param->name = parse_id();
            expect(':');
            param->type = parse_type();
        }
        param->next = NULL;
        next();
        return head;
    }
    else {
        next();
        return NULL;
    }
}

// Initialiser, optional `= <expr>` after a declaration
struct expr* parse_initialiser(void) {
    if (match(TOK_ASSIGN)) {
        next();
        return parse_expr();
    }
    return NULL;
}

// Type
struct type* parse_type(void) {
    if (match('*')) {
        next();
        struct type* type = malloc(sizeof(struct type));
        type->kind = 1; // pointer
        type->base = parse_type();
        return type;
    }
    if (match('&')) {
        next();
        struct type* type = malloc(sizeof(struct type));
        type->kind = 2; // reference
        type->base = parse_type();
        return type;
    }

    struct type* type = malloc(sizeof(struct type));
    type->kind = 0;
    type->name = parse_id();
    return type;
}

// Identifier
struct token* parse_id(void) {
    struct token* token = curr_token;
    expect(TOK_ID);
    return token;
}
