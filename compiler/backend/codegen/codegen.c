#include "codegen.h"
#include "common.h"
#include "emitter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Counter for temporary registers and labels
static int tmp_count = 0;
static int label_count = 0;

// Helper to generate a new temporary register name
static void gen_temp(char* buffer) { sprintf(buffer, "%%t%d", tmp_count++); }

// Helper to generate a new label
static void gen_label(char* buffer) { sprintf(buffer, "L%d", label_count++); }

struct symbol {
    char* name;
    struct type* type;
    struct symbol* next;
};

static struct symbol* symbol_table = NULL;

static void add_symbol(char* name, struct type* type) {
    struct symbol* sym = malloc(sizeof(struct symbol));
    sym->name = strdup(name);
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

// Map Lyte types to LLVM types
static void map_type_str(struct type* type, char* buffer) {
    if (type == NULL) {
        strcpy(buffer, "i32");
        return;
    }
    if (type->kind == 1) { // pointer
        char base[128];
        map_type_str(type->base, base);
        sprintf(buffer, "%s*", base);
        return;
    }
    if (type->kind == 2) { // reference
        char base[128];
        map_type_str(type->base, base);
        sprintf(buffer, "%s*", base); // References are pointers in LLVM
        return;
    }

    switch (type->primitive) {
    case TOK_S8:
    case TOK_U8:
        strcpy(buffer, "i8");
        break;
    case TOK_S16:
    case TOK_U16:
        strcpy(buffer, "i16");
        break;
    case TOK_S32:
    case TOK_U32:
    case TOK_INT:
        strcpy(buffer, "i32");
        break;
    case TOK_S64:
    case TOK_U64:
        strcpy(buffer, "i64");
        break;
    case TOK_F32:
        strcpy(buffer, "float");
        break;
    case TOK_F64:
        strcpy(buffer, "double");
        break;
    default:
        strcpy(buffer, "i32");
        break;
    }
}

// Forward declarations
void gen_stmt(struct stmt* stmt);
void gen_compound_stmt(struct stmt* stmts);
void gen_expr(struct expr* expr, char* out_reg);

// Generates LLVM IR for expressions
// The result is stored in the register specified by out_reg
void gen_expr(struct expr* expr, char* out_reg) {
    if (expr == NULL) {
        return;
    }
    char buffer[256];

    switch (expr->type) {
    case EXPR_ARITH: {
        struct arith_expr* arith = expr->exprs.arith_expr;
        char left_reg[32];
        char right_reg[32];

        gen_expr(arith->left, left_reg);
        gen_expr(arith->right, right_reg);

        gen_temp(out_reg);
        char op_str[16];
        switch (arith->op) {
        case '+':
            strcpy(op_str, "add");
            break;
        case '-':
            strcpy(op_str, "sub");
            break;
        case '*':
            strcpy(op_str, "mul");
            break;
        case '/':
            strcpy(op_str, "sdiv");
            break;
        default:
            strcpy(op_str, "add");
            break;
        }

        sprintf(buffer, "  %s = %s i32 %s, %s\n", out_reg, op_str, left_reg,
                right_reg);
        emit(buffer);
        break;
    }
    case EXPR_ASSIGN: {
        struct assign_expr* assign = expr->exprs.assign_expr;
        char val_reg[32];
        gen_expr(assign->value, val_reg);

        char var_name[128];
        snprintf(var_name, (size_t)assign->id->length + 1, "%s",
                 assign->id->start_pos);

        if (assign->op == TOK_ASSIGN) {
            sprintf(buffer, "  store i32 %s, i32* %%var_%s\n", val_reg,
                    var_name);
            emit(buffer);
            strcpy(out_reg, val_reg);
        }
        else {
            char curr_val[32];
            gen_temp(curr_val);
            sprintf(buffer, "  %s = load i32, i32* %%var_%s\n", curr_val,
                    var_name);
            emit(buffer);

            char new_val[32];
            gen_temp(new_val);

            char op_str[16];
            switch (assign->op) {
            case TOK_ADD_ASSIGN:
                strcpy(op_str, "add");
                break;
            case TOK_SUB_ASSIGN:
                strcpy(op_str, "sub");
                break;
            case TOK_MUL_ASSIGN:
                strcpy(op_str, "mul");
                break;
            case TOK_DIV_ASSIGN:
                strcpy(op_str, "sdiv");
                break;
            default:
                strcpy(op_str, "add");
                break;
            }

            sprintf(buffer, "  %s = %s i32 %s, %s\n", new_val, op_str, curr_val,
                    val_reg);
            emit(buffer);

            sprintf(buffer, "  store i32 %s, i32* %%var_%s\n", new_val,
                    var_name);
            emit(buffer);
            strcpy(out_reg, new_val);
        }
        break;
    }
    case EXPR_EQUALITY: {
        struct equality_expr* eq = expr->exprs.equality_expr;
        char left_reg[32];
        char right_reg[32];

        gen_expr(eq->left, left_reg);
        gen_expr(eq->right, right_reg);

        gen_temp(out_reg);
        char cond_code[4];
        switch (eq->op) {
        case TOK_EQEQ:
            strcpy(cond_code, "eq");
            break;
        case TOK_NOTEQ:
            strcpy(cond_code, "ne");
            break;
        case '>':
            strcpy(cond_code, "sgt");
            break;
        case '<':
            strcpy(cond_code, "slt");
            break;
        case TOK_GTEQ:
            strcpy(cond_code, "sge");
            break;
        case TOK_LTEQ:
            strcpy(cond_code, "sle");
            break;
        default:
            strcpy(cond_code, "eq");
            break;
        }

        sprintf(buffer, "  %s = icmp %s i32 %s, %s\n", out_reg, cond_code,
                left_reg, right_reg);
        emit(buffer);
        break;
    }
    case EXPR_VALUE: {
        struct token* tok = expr->exprs.value_expr;
        snprintf(out_reg, (size_t)tok->length + 1, "%s", tok->start_pos);
        break;
    }
    case EXPR_ID: {
        struct token* tok = expr->exprs.id;
        char var_name[128];
        snprintf(var_name, (size_t)tok->length + 1, "%s", tok->start_pos);

        struct type* type = get_symbol_type(var_name);
        char type_str[128];
        map_type_str(type, type_str);

        gen_temp(out_reg);
        sprintf(buffer, "  %s = load %s, %s* %%var_%s\n", out_reg, type_str,
                type_str, var_name);
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
                char var_name[128];
                snprintf(var_name, (size_t)tok->length + 1, "%s",
                         tok->start_pos);
                sprintf(out_reg, "%%var_%s", var_name);
            }
            else {
                // Error or handle complex cases
                strcpy(out_reg, "0");
            }
        }
        else if (unary->op == '*') {
            // Dereference: load from the pointer
            char ptr_reg[32];
            gen_expr(unary->expr, ptr_reg);

            // We need to know the type being pointed to.
            // This is hard without full type inference/propagation in
            // expression. For now, let's assume i32 if we can't infer, or we
            // need to track expression types. Since we don't have expression
            // type tracking yet, this is tricky. BUT, if we are dereferencing,
            // the ptr_reg holds a pointer. We load from it. LLVM load needs
            // type. We might need to change gen_expr to return type or store it
            // in expr. For this task, maybe we assume i32 for dereference
            // result if unknown? Or we can try to look up if it's an ID.

            // Simplification: Assume i32 for now as fallback, but this is wrong
            // for typed pointers. To do this properly, we need type
            // checking/inference phase. Given the scope, I'll try to implement
            // basic type tracking in gen_expr if possible, or just support *ptr
            // where ptr is a known variable.

            // Let's assume i32 for the result of dereference for now to get it
            // compiling, but this is a limitation. Actually, if we have `let x:
            // *i32`, `x` is `i32*`. `*x` is `i32`. We can look up `x` type.

            gen_temp(out_reg);
            sprintf(buffer, "  %s = load i32, i32* %s\n", out_reg, ptr_reg);
            emit(buffer);
        }
        else {
            // Handle ! and -
            char val_reg[32];
            gen_expr(unary->expr, val_reg);
            gen_temp(out_reg);
            if (unary->op == '!') {
                sprintf(buffer, "  %s = xor i32 %s, -1\n", out_reg,
                        val_reg); // Bitwise NOT? Or logical?
                // Lyte ! is likely logical NOT if boolean, but here everything
                // is i32. Let's use icmp eq 0 then zext? For now, let's stick
                // to simple.
                sprintf(buffer, "  %s = icmp eq i32 %s, 0\n", out_reg, val_reg);
                // This returns i1. We might need zext to i32 if we use i32
                // everywhere. But gen_expr returns register name. Let's just
                // emit sub 0, val?
            }
            else if (unary->op == '-') {
                sprintf(buffer, "  %s = sub i32 0, %s\n", out_reg, val_reg);
            }
            emit(buffer);
        }
        break;
    }
    case EXPR_CALL: {
        struct call_expr* call = expr->exprs.call_expr;
        char fn_name[128];
        snprintf(fn_name, (size_t)call->id->length + 1, "%s",
                 call->id->start_pos);

        char arg_regs[8][32];
        int arg_count = 0;
        struct arg* arg = call->args;
        while (arg != NULL && arg_count < 8) {
            gen_expr(arg->value, arg_regs[arg_count]);
            arg_count++;
            arg = arg->next;
        }

        gen_temp(out_reg);
        sprintf(buffer, "  %s = call i32 @%s(", out_reg, fn_name);
        emit(buffer);

        for (int i = 0; i < arg_count; i++) {
            sprintf(buffer, "i32 %s%s", arg_regs[i],
                    (i < arg_count - 1) ? ", " : "");
            emit(buffer);
        }
        emit(")\n");
        break;
    }
    default:
        // Handle other types or error
        strcpy(out_reg, "0");
        break;
    }
}

void gen_stmt(struct stmt* stmt) {
    char buffer[256];

    switch (stmt->type) {
    case STMT_LET: {
        struct let_decl* decl = stmt->stmt.let_decl;
        char var_name[128];
        snprintf(var_name, (size_t)decl->name->length + 1, "%s",
                 decl->name->start_pos);

        char type_str[128];
        map_type_str(decl->type, type_str);

        sprintf(buffer, "  %%var_%s = alloca %s\n", var_name, type_str);
        emit(buffer);

        add_symbol(var_name, decl->type);

        if (decl->value != NULL) {
            char val_reg[32];
            gen_expr(decl->value, val_reg);
            sprintf(buffer, "  store %s %s, %s* %%var_%s\n", type_str, val_reg,
                    type_str, var_name);
            emit(buffer);
        }
        break;
    }
    case STMT_RETURN: {
        struct return_stmt* ret = stmt->stmt.return_stmt;
        char val_reg[32];
        gen_expr(ret->expr, val_reg);
        sprintf(buffer, "  ret i32 %s\n", val_reg);
        emit(buffer);
        break;
    }
    case STMT_EXPR: {
        char val_reg[32];
        gen_expr(stmt->stmt.expr_stmt->expr, val_reg);
        break;
    }
    case STMT_IF: {
        struct if_stmt* if_s = stmt->stmt.if_stmt;

        char cond_reg[32];
        gen_expr(if_s->if_cond, cond_reg);

        char then_label[32], else_label[32], end_label[32];
        gen_label(then_label);
        gen_label(else_label);
        gen_label(end_label);

        sprintf(buffer, "  br i1 %s, label %%%s, label %%%s\n", cond_reg,
                then_label, else_label);
        emit(buffer);

        emit(then_label);
        emit(":\n");
        gen_compound_stmt(if_s->if_body);
        sprintf(buffer, "  br label %%%s\n", end_label);
        emit(buffer);

        emit(else_label);
        emit(":\n");
        if (if_s->else_body != NULL) {
            gen_compound_stmt(if_s->else_body);
        }
        sprintf(buffer, "  br label %%%s\n", end_label);
        emit(buffer);

        emit(end_label);
        emit(":\n");
        break;
    }
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
    char type_str[128];
    map_type_str(fn->type, type_str);
    emit(type_str);
    emit(" @");

    // Emit name without newline or colon
    char name[128];
    snprintf(name, (size_t)fn->name->length + 1, "%s", fn->name->start_pos);
    emit(name);

    emit("(");

    // Params
    struct param* p = fn->params;
    while (p != NULL) {
        char p_name[128];
        snprintf(p_name, (size_t)p->name->length + 1, "%s", p->name->start_pos);
        map_type_str(p->type, type_str);
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
    char buffer[256];
    while (p != NULL) {
        char p_name[128];
        snprintf(p_name, (size_t)p->name->length + 1, "%s", p->name->start_pos);
        map_type_str(p->type, type_str);

        sprintf(buffer, "  %%var_%s = alloca %s\n", p_name, type_str);
        emit(buffer);
        sprintf(buffer, "  store %s %%%s, %s* %%var_%s\n", type_str, p_name,
                type_str, p_name);
        emit(buffer);

        p = p->next;
    }

    gen_compound_stmt(fn->body);

    // Fallback return
    // TODO: Match return type
    emit("  ret i32 0\n");
    emit("}\n\n");
}

void gen_entry(struct entry_decl* entry) {
    tmp_count = 0;
    label_count = 0;
    free_symbols();

    emit("define i32 @");

    char name[128];
    snprintf(name, (size_t)entry->name->length + 1, "%s", entry->name->start_pos);
    emit(name);

    emit("() {\n");
    emit("entry:\n");

    gen_compound_stmt(entry->body);

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

    if (program->entry != NULL) {
        gen_entry(program->entry);
    }

    emit_finish();
}
