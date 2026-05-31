#include "codegen.h"
#include "common.h"
#include "emitter.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REG_BUF_SIZE 32
#define TYPE_BUF_SIZE 128
#define NAME_BUF_SIZE 128
#define IR_BUF_SIZE 256

// Counter for temporary registers and labels
static int tmp_count = 0;
static int label_count = 0;

static void write_fmt(char* buffer, size_t size, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, size, fmt, args);
    va_end(args);
}

static void copy_str(char* buffer, size_t size, const char* value) { write_fmt(buffer, size, "%s", value); }

static char* duplicate_str(const char* value) {
    size_t len = strlen(value) + 1;
    char* copy = malloc(len);
    if (copy != NULL) {
        memcpy(copy, value, len);
    }
    return copy;
}

// Helper to generate a new temporary register name
static void gen_temp(char* buffer, size_t size) { write_fmt(buffer, size, "%%t%d", tmp_count++); }

// Helper to generate a new label
static void gen_label(char* buffer, size_t size) { write_fmt(buffer, size, "L%d", label_count++); }

struct symbol {
    char* name;
    struct type* type;
    struct symbol* next;
};

static struct symbol* symbol_table = NULL;

static void add_symbol(char* name, struct type* type) {
    struct symbol* sym = malloc(sizeof(struct symbol));
    sym->name = duplicate_str(name);
    sym->type = type;
    sym->next = symbol_table;
    symbol_table = sym;
}

static struct type* get_symbol_type(char* name) {
    struct symbol* sym = symbol_table;
    while (sym != NULL) {
        if (strcmp(sym->name, name) == 0) {
            return sym->type;
        }
        sym = sym->next;
    }
    return NULL;
}

static void free_symbols(void) {
    while (symbol_table != NULL) {
        struct symbol* next = symbol_table->next;
        free(symbol_table->name);
        free(symbol_table);
        symbol_table = next;
    }
}

static int type_name_is(struct type* type, const char* name) {
    return type->name != NULL && type->name->length == (int)strlen(name) &&
           strncmp(type->name->start_pos, name, (size_t)type->name->length) == 0;
}

// Map Lyte types to LLVM types
static void map_type_str(struct type* type, char* buffer, size_t size) {
    if (type == NULL) {
        copy_str(buffer, size, "i32");
        return;
    }
    if (type->kind == 1) { // pointer
        char base[TYPE_BUF_SIZE];
        map_type_str(type->base, base, sizeof(base));
        write_fmt(buffer, size, "%s*", base);
        return;
    }
    if (type->kind == 2) { // reference
        char base[TYPE_BUF_SIZE];
        map_type_str(type->base, base, sizeof(base));
        write_fmt(buffer, size, "%s*", base); // References are pointers in LLVM
        return;
    }

    if (type->name == NULL) {
        copy_str(buffer, size, "i32");
        return;
    }

    if (type_name_is(type, "s8") || type_name_is(type, "u8")) {
        copy_str(buffer, size, "i8");
    }
    else if (type_name_is(type, "s16") || type_name_is(type, "u16")) {
        copy_str(buffer, size, "i16");
    }
    else if (type_name_is(type, "s32") || type_name_is(type, "u32")) {
        copy_str(buffer, size, "i32");
    }
    else if (type_name_is(type, "s64") || type_name_is(type, "u64")) {
        copy_str(buffer, size, "i64");
    }
    else if (type_name_is(type, "ssize") || type_name_is(type, "usize")) {
        copy_str(buffer, size, "i64");
    }
    else if (type_name_is(type, "f32")) {
        copy_str(buffer, size, "float");
    }
    else if (type_name_is(type, "f64")) {
        copy_str(buffer, size, "double");
    }
    else if (type_name_is(type, "f128")) {
        copy_str(buffer, size, "fp128");
    }
    else {
        copy_str(buffer, size, "i32");
    }
}

// Forward declarations
void gen_stmt(struct stmt* stmt);
void gen_compound_stmt(struct stmt* stmts);
void gen_expr(struct expr* expr, char* out_reg, size_t out_size);

// Generates LLVM IR for expressions
// The result is stored in the register specified by out_reg
void gen_expr(struct expr* expr, char* out_reg, size_t out_size) {
    if (expr == NULL) {
        return;
    }
    char buffer[IR_BUF_SIZE];

    switch (expr->type) {
    case EXPR_ARITH: {
        struct arith_expr* arith = expr->exprs.arith_expr;
        char left_reg[REG_BUF_SIZE];
        char right_reg[REG_BUF_SIZE];

        gen_expr(arith->left, left_reg, sizeof(left_reg));
        gen_expr(arith->right, right_reg, sizeof(right_reg));

        gen_temp(out_reg, out_size);
        const char* op_str;
        switch (arith->op) {
        case '+':
            op_str = "add";
            break;
        case '-':
            op_str = "sub";
            break;
        case '*':
            op_str = "mul";
            break;
        case '/':
            op_str = "sdiv";
            break;
        default:
            op_str = "add";
            break;
        }

        write_fmt(buffer, sizeof(buffer), "  %s = %s i32 %s, %s\n", out_reg, op_str, left_reg, right_reg);
        emit(buffer);
        break;
    }
    case EXPR_ASSIGN: {
        struct assign_expr* assign = expr->exprs.assign_expr;
        char val_reg[REG_BUF_SIZE];
        gen_expr(assign->value, val_reg, sizeof(val_reg));

        char var_name[NAME_BUF_SIZE];
        snprintf(var_name, sizeof(var_name), "%.*s", assign->id->length, assign->id->start_pos);

        if (assign->op == TOK_ASSIGN) {
            write_fmt(buffer, sizeof(buffer), "  store i32 %s, i32* %%var_%s\n", val_reg, var_name);
            emit(buffer);
            copy_str(out_reg, out_size, val_reg);
        }
        else {
            char curr_val[REG_BUF_SIZE];
            gen_temp(curr_val, sizeof(curr_val));
            write_fmt(buffer, sizeof(buffer), "  %s = load i32, i32* %%var_%s\n", curr_val, var_name);
            emit(buffer);

            char new_val[REG_BUF_SIZE];
            gen_temp(new_val, sizeof(new_val));

            const char* op_str;
            switch (assign->op) {
            case TOK_ADD_ASSIGN:
                op_str = "add";
                break;
            case TOK_SUB_ASSIGN:
                op_str = "sub";
                break;
            case TOK_MUL_ASSIGN:
                op_str = "mul";
                break;
            case TOK_DIV_ASSIGN:
                op_str = "sdiv";
                break;
            default:
                op_str = "add";
                break;
            }

            write_fmt(buffer, sizeof(buffer), "  %s = %s i32 %s, %s\n", new_val, op_str, curr_val, val_reg);
            emit(buffer);

            write_fmt(buffer, sizeof(buffer), "  store i32 %s, i32* %%var_%s\n", new_val, var_name);
            emit(buffer);
            copy_str(out_reg, out_size, new_val);
        }
        break;
    }
    case EXPR_EQUALITY: {
        struct equality_expr* eq = expr->exprs.equality_expr;
        char left_reg[REG_BUF_SIZE];
        char right_reg[REG_BUF_SIZE];

        gen_expr(eq->left, left_reg, sizeof(left_reg));
        gen_expr(eq->right, right_reg, sizeof(right_reg));

        gen_temp(out_reg, out_size);
        const char* cond_code;
        switch (eq->op) {
        case TOK_EQEQ:
            cond_code = "eq";
            break;
        case TOK_NOTEQ:
            cond_code = "ne";
            break;
        case '>':
            cond_code = "sgt";
            break;
        case '<':
            cond_code = "slt";
            break;
        case TOK_GTEQ:
            cond_code = "sge";
            break;
        case TOK_LTEQ:
            cond_code = "sle";
            break;
        default:
            cond_code = "eq";
            break;
        }

        write_fmt(buffer, sizeof(buffer), "  %s = icmp %s i32 %s, %s\n", out_reg, cond_code, left_reg, right_reg);
        emit(buffer);
        break;
    }
    case EXPR_VALUE: {
        struct token* tok = expr->exprs.value_expr;
        snprintf(out_reg, out_size, "%.*s", tok->length, tok->start_pos);
        break;
    }
    case EXPR_ID: {
        struct token* tok = expr->exprs.id;
        char var_name[NAME_BUF_SIZE];
        snprintf(var_name, sizeof(var_name), "%.*s", tok->length, tok->start_pos);

        struct type* type = get_symbol_type(var_name);
        char type_str[TYPE_BUF_SIZE];
        map_type_str(type, type_str, sizeof(type_str));

        gen_temp(out_reg, out_size);
        write_fmt(buffer, sizeof(buffer), "  %s = load %s, %s* %%var_%s\n", out_reg, type_str, type_str, var_name);
        emit(buffer);
        break;
    }
    case EXPR_UNARY: {
        struct unary_expr* unary = expr->exprs.unary_expr;
        if (unary->op == '&') {
            // Address of: return the alloca pointer directly
            // We assume the operand is an ID for now
            if (unary->expr->type == EXPR_ID) {
                struct token* tok = unary->expr->exprs.id;
                char var_name[NAME_BUF_SIZE];
                snprintf(var_name, sizeof(var_name), "%.*s", tok->length, tok->start_pos);
                write_fmt(out_reg, out_size, "%%var_%s", var_name);
            }
            else {
                // Error or handle complex cases
                copy_str(out_reg, out_size, "0");
            }
        }
        else if (unary->op == '*') {
            // Dereference: load from the pointer
            char ptr_reg[REG_BUF_SIZE];
            gen_expr(unary->expr, ptr_reg, sizeof(ptr_reg));

            // Assume i32 for now.
            gen_temp(out_reg, out_size);
            write_fmt(buffer, sizeof(buffer), "  %s = load i32, i32* %s\n", out_reg, ptr_reg);
            emit(buffer);
        }
        else {
            // Handle ! and -
            char val_reg[REG_BUF_SIZE];
            gen_expr(unary->expr, val_reg, sizeof(val_reg));
            gen_temp(out_reg, out_size);
            if (unary->op == '!') {
                write_fmt(buffer, sizeof(buffer), "  %s = icmp eq i32 %s, 0\n", out_reg, val_reg);
            }
            else if (unary->op == '-') {
                write_fmt(buffer, sizeof(buffer), "  %s = sub i32 0, %s\n", out_reg, val_reg);
            }
            emit(buffer);
        }
        break;
    }
    case EXPR_CALL: {
        struct call_expr* call = expr->exprs.call_expr;
        char fn_name[NAME_BUF_SIZE];
        if (call->name != NULL) {
            copy_str(fn_name, sizeof(fn_name), call->name);
        }
        else {
            snprintf(fn_name, sizeof(fn_name), "%.*s", call->id->length, call->id->start_pos);
        }

        char arg_regs[8][REG_BUF_SIZE];
        int arg_count = 0;
        struct arg* arg = call->args;
        while (arg != NULL && arg_count < 8) {
            gen_expr(arg->value, arg_regs[arg_count], sizeof(arg_regs[0]));
            arg_count++;
            arg = arg->next;
        }

        gen_temp(out_reg, out_size);
        write_fmt(buffer, sizeof(buffer), "  %s = call i32 @%s(", out_reg, fn_name);
        emit(buffer);

        for (int i = 0; i < arg_count; i++) {
            write_fmt(buffer, sizeof(buffer), "i32 %s%s", arg_regs[i], (i < arg_count - 1) ? ", " : "");
            emit(buffer);
        }
        emit(")\n");
        break;
    }
    default:
        // Handle other types or error
        copy_str(out_reg, out_size, "0");
        break;
    }
}

void gen_stmt(struct stmt* stmt) {
    char buffer[IR_BUF_SIZE];

    switch (stmt->type) {
    case STMT_VAR: {
        struct var_decl* decl = stmt->stmt.var_decl;
        char var_name[NAME_BUF_SIZE];
        snprintf(var_name, sizeof(var_name), "%.*s", decl->name->length, decl->name->start_pos);

        char type_str[TYPE_BUF_SIZE];
        map_type_str(decl->type, type_str, sizeof(type_str));

        write_fmt(buffer, sizeof(buffer), "  %%var_%s = alloca %s\n", var_name, type_str);
        emit(buffer);

        add_symbol(var_name, decl->type);

        if (decl->value != NULL) {
            char val_reg[REG_BUF_SIZE];
            gen_expr(decl->value, val_reg, sizeof(val_reg));
            write_fmt(buffer, sizeof(buffer), "  store %s %s, %s* %%var_%s\n", type_str, val_reg, type_str, var_name);
            emit(buffer);
        }
        break;
    }
    case STMT_RETURN: {
        struct return_stmt* ret = stmt->stmt.return_stmt;
        char val_reg[REG_BUF_SIZE];
        gen_expr(ret->expr, val_reg, sizeof(val_reg));
        write_fmt(buffer, sizeof(buffer), "  ret i32 %s\n", val_reg);
        emit(buffer);
        break;
    }
    case STMT_EXPR: {
        char val_reg[REG_BUF_SIZE];
        gen_expr(stmt->stmt.expr_stmt->expr, val_reg, sizeof(val_reg));
        break;
    }
    case STMT_IF: {
        struct if_stmt* if_s = stmt->stmt.if_stmt;

        char cond_reg[REG_BUF_SIZE];
        gen_expr(if_s->if_cond, cond_reg, sizeof(cond_reg));

        char then_label[REG_BUF_SIZE], else_label[REG_BUF_SIZE], end_label[REG_BUF_SIZE];
        gen_label(then_label, sizeof(then_label));
        gen_label(else_label, sizeof(else_label));
        gen_label(end_label, sizeof(end_label));

        write_fmt(buffer, sizeof(buffer), "  br i1 %s, label %%%s, label %%%s\n", cond_reg, then_label, else_label);
        emit(buffer);

        emit(then_label);
        emit(":\n");
        gen_compound_stmt(if_s->if_body);
        write_fmt(buffer, sizeof(buffer), "  br label %%%s\n", end_label);
        emit(buffer);

        emit(else_label);
        emit(":\n");
        if (if_s->else_body != NULL) {
            gen_compound_stmt(if_s->else_body);
        }
        write_fmt(buffer, sizeof(buffer), "  br label %%%s\n", end_label);
        emit(buffer);

        emit(end_label);
        emit(":\n");
        break;
    }
    case STMT_FOR: {
        struct for_stmt* for_s = stmt->stmt.for_stmt;
        if (for_s->init != NULL) {
            gen_stmt(for_s->init);
        }
        gen_compound_stmt(for_s->body);
        break;
    }
    case STMT_UNSAFE:
        gen_compound_stmt(stmt->stmt.unsafe_stmt->body);
        break;
    default:
        // Handle other statements
        break;
    }
}

void gen_compound_stmt(struct stmt* stmts) {
    struct stmt* s = stmts;
    while (s != NULL) {
        gen_stmt(s);
        s = s->next;
    }
}

void gen_fn(struct fn_decl* fn) {
    tmp_count = 0;
    label_count = 0;
    free_symbols(); // Clear symbols from previous function

    emit("define ");
    char type_str[TYPE_BUF_SIZE];
    map_type_str(fn->type, type_str, sizeof(type_str));
    emit(type_str);
    emit(" @");

    // Emit name without newline or colon
    char name[NAME_BUF_SIZE];
    snprintf(name, sizeof(name), "%.*s", fn->name->length, fn->name->start_pos);
    emit(name);

    emit("(");

    // Params
    struct param* p = fn->params;
    while (p != NULL) {
        char p_name[NAME_BUF_SIZE];
        snprintf(p_name, sizeof(p_name), "%.*s", p->name->length, p->name->start_pos);
        map_type_str(p->type, type_str, sizeof(type_str));
        emit(type_str);
        emit(" %");
        emit(p_name);

        add_symbol(p_name, p->type);

        if (p->next != NULL)
            emit(", ");
        p = p->next;
    }
    emit(") {\n");
    emit("entry:\n");

    // Alloc params to stack so they are mutable
    p = fn->params;
    char buffer[IR_BUF_SIZE];
    while (p != NULL) {
        char p_name[NAME_BUF_SIZE];
        snprintf(p_name, sizeof(p_name), "%.*s", p->name->length, p->name->start_pos);
        map_type_str(p->type, type_str, sizeof(type_str));

        write_fmt(buffer, sizeof(buffer), "  %%var_%s = alloca %s\n", p_name, type_str);
        emit(buffer);
        write_fmt(buffer, sizeof(buffer), "  store %s %%%s, %s* %%var_%s\n", type_str, p_name, type_str, p_name);
        emit(buffer);

        p = p->next;
    }

    gen_compound_stmt(fn->body);

    // Fallback return
    // TODO: Match return type
    emit("  ret i32 0\n");
    emit("}\n\n");
}

void gen(char* bin_name, struct program_ast* program) {
    emitter(bin_name);

    struct fn_decl* fn = program->fn_decls;
    while (fn != NULL) {
        gen_fn(fn);
        fn = fn->next;
    }

    emit_finish();
}
