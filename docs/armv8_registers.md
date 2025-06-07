sp -> stack pointer
wzr/xzr -> zero register
w0 - w30 -> 32bits general purpose (can be use to pass parameters and return values)
x0 - x30 -> same as above, except 64 bits
- clang reserver w0 to w7 for parameters, gcc doesn't 