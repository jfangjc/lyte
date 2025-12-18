#include "gen.h"
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

// Map Lyte types to LLVM types
static const char* map_type(int type) {
    switch (type) {
    case TOK_S8:
    case TOK_U8:
        return "i8";
    case TOK_S16:
    case TOK_U16:
        return "i16";
    case TOK_S32:
    case TOK_U32:
    case TOK_INT:
        return "i32";
    case TOK_S64:
    case TOK_U64:
        return "i64";
    case TOK_S128:
    case TOK_U128:
        return "i128";
    case TOK_F32:
        return "float";
    case TOK_F64:
        return "double";
    case TOK_F128:
        return "fp128";
    default:
        return "i32";
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

        gen_temp(out_reg);
        sprintf(buffer, "  %s = load i32, i32* %%var_%s\n", out_reg, var_name);
        emit(buffer);
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

        sprintf(buffer, "  %%var_%s = alloca i32\n", var_name);
        emit(buffer);

        if (decl->value != NULL) {
            char val_reg[32];
            gen_expr(decl->value, val_reg);
            sprintf(buffer, "  store i32 %s, i32* %%var_%s\n", val_reg,
                    var_name);
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

    emit("define ");
    emit(map_type(fn->type));
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
        emit("i32 %");
        emit(p_name);
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

        sprintf(buffer, "  %%var_%s = alloca i32\n", p_name);
        emit(buffer);
        sprintf(buffer, "  store i32 %%%s, i32* %%var_%s\n", p_name, p_name);
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
