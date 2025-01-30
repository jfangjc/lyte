int* text;
int* old_text;
int* stack;
char* data;

int* pc;
int* sp;
int* bp;

int ax;

void set_pc(int* value) {
    pc = value;
}

void set_sp(int* value) {
    sp = value;
}

void set_bp(int* value) {
    bp = value;
}

void set_ax(int value) {
    ax = value;
}

int* get_pc() {
    return pc;
}

int* get_sp() {
    return sp;
}

int* get_bp() {
    return bp;
}

int get_ax() {
    return ax;
}